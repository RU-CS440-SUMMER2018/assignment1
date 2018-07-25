/**
 * @file application.cpp
 *
 * @copyright Software License Agreement (BSD License)
 * Copyright (c) 2013, Rutgers the State University of New Jersey, New Brunswick 
 * All Rights Reserved.
 * For a full description see the file named LICENSE.
 *
 * Authors: Andrew Dobson, Andrew Kimmel, Athanasios Krontiris, Zakary Littlefield, Kostas Bekris
 *
 * Email: pracsys@googlegroups.com
 */

#include "prx/utilities/applications/application.hpp"
#include "prx/utilities/parameters/parameter_reader.hpp"
#include "prx/utilities/definitions/string_manip.hpp"
#include "prx/utilities/communication/communication.hpp"

#include "prx/utilities/communication/tf_broadcaster.hpp"

// #include <boost/range/adaptor/map.hpp> //adaptors
#include <fstream>
#include "prx/utilities/definitions/sys_clock.hpp"
#include <pluginlib/class_list_macros.h>
#include "prx_core/send_plants_srv.h"
#include "prx_core/visualize_obstacles_srv.h"
#include "prx/utilities/definitions/random.hpp"
#include <boost/assign/list_of.hpp>
#include "prx/utilities/graph/undirected_node.hpp"

#include <iostream>

PLUGINLIB_EXPORT_CLASS(prx::util::util_application_t, prx::util::util_application_t)

namespace prx
{

    namespace util
    {
        //This is needed for pracsys to be able to load this class when we start a node.
        pluginlib::ClassLoader<util_application_t> util_application_t::loader("prx_core", "prx::util::util_application_t");

        util_application_t::util_application_t()
        {
            tf_broadcaster = NULL;
            agent_state = START;
            start_i = 0;
            start_j = 0;
            searcher = new search_t();
            init_random(1);
        }

        util_application_t::~util_application_t()
        {
            delete tf_broadcaster;
            delete searcher;
        }

        void util_application_t::init(const parameter_reader_t * const reader)
        {

            //create the tf broadcaster, which tells the visualization node where all of the geometries are placed in the world
            tf_broadcaster = new tf_broadcaster_t;
            ros::ServiceClient plant_client = node_handle.serviceClient<prx_core::send_plants_srv > ("visualization/plants");

            prx_core::send_plants_srv plant_wrapper;
            plant_wrapper.request.source_node_name = "utilities";
            plant_wrapper.request.paths.push_back("simulator/disk");
            plant_wrapper.request.paths.push_back("simulator/goal");
            plant_client.waitForExistence(ros::Duration(5));
            if( !plant_client.call(plant_wrapper) )
            {
                PRX_FATAL_S("Service request to send plants failed.");
            }

            //set the initial location of the robot
            auto start_pose = pose_from_indices(start_i, start_j);
            _x = start_pose.first;
            _y = start_pose.second;

            current_goal_x = _x;
            current_goal_y = _y;

            _z = 0.5;

            _qx = _qy = _qz = 0;
            _qw = 1;

            //initialize the state memory for the ball (where the robot can go)
            std::vector<double*> state_memory = {&_x, &_y, &_z, &_qx, &_qy, &_qz, &_qw};

            state_space = new space_t("SE3", state_memory);
            //The ball can only move within these bounds
            state_space->get_bounds()[0]->set_bounds(-r-2, r+2);
            state_space->get_bounds()[1]->set_bounds(-c-2, c+2);
            state_space->get_bounds()[2]->set_bounds(-2, 4);


            ros::ServiceClient obs_client = node_handle.serviceClient<prx_core::visualize_obstacles_srv > ("visualization/visualize_obstacles");
            prx_core::visualize_obstacles_srv obs_wrapper;
            obs_wrapper.request.obstacles_path = "utilities";
            obs_client.waitForExistence(ros::Duration(5));
            if( !obs_client.call(obs_wrapper) )
            {
                PRX_FATAL_S("Service request to send obstacles failed.");
            }
        }

        std::pair<int, int> util_application_t::pose_from_indices(int i, int j)
        {
            return std::make_pair(j, -i);
        }

        std::pair<int, int> util_application_t::indices_from_pose(int x, int y)
        {
            return std::make_pair(-y, x);
        }

        void util_application_t::build_environment(std::string filename)
        {
            char* w = std::getenv("PRACSYS_PATH");
            std::string block_file(w);
            block_file += ("/prx_core/launches/block.yaml");
            PRX_PRINT("The obstacle template is "<<block_file, PRX_TEXT_RED);

            environment_file = filename;
            PRX_INFO_S("File directory is: " << filename);
            std::ifstream fin;

            try
            {
                fin.open(filename);
                if(!fin.good())
                {
                    PRX_FATAL_S("Error in trying to read file "<<filename);
                }
                PRX_PRINT("Opened file..", PRX_TEXT_MAGENTA);

                fin >> r;
                fin >> c;
                PRX_PRINT("The read attributes: "<<r<<" "<<c<<" ", PRX_TEXT_MAGENTA);

                maze.resize(r);
                int num_obs = 1;
                int total_cells = r*c;
                int progress = 0;

                for(int i=0; i<r; ++i)
                {
                    maze[i].resize(c,-1);
                    for(int j=0; j<c; ++j, ++progress)
                    {
                        // PRX_PRINT("Reading index: "<<i<<", "<<j<<" to "<<maze[i][j], PRX_TEXT_MAGENTA);
                        fin >> maze[i][j];
                        // std::cout<<maze[i][j]<<" ";
                        if(maze[i][j] == 0)
                        {
                            auto pose = pose_from_indices(i,j);
                            std::vector<double> obs_pos = {(double)pose.first, (double)pose.second, 0.5};
                            std::string command = "rosparam load "+block_file+" /utilities/obstacles/block_"+std::to_string(num_obs);
                            auto ret = std::system(command.c_str());
                            ros::param::set("/utilities/obstacles/block_"+std::to_string(num_obs)+"/root_configuration/position", obs_pos);
                            num_obs++;
                        }
                        PRX_STATUS("Constructing environment: "<<progress*100/(double)total_cells<<"%       ", PRX_TEXT_LIGHTGRAY);
                    }
                    // std::cout<<"\n";
                }

            }
            catch(...)
            {
                PRX_FATAL_S("Could not parse file.");
            }

            PRX_PRINT("Printing the read maze:", PRX_TEXT_MAGENTA);
            std::cout<<"\n-----------------------------\n";
            for(int i=0; i<r; ++i)
            {
                std::cout<<"\n";
                for(int j=0; j<c; ++j)
                {
                    std::cout<<maze[i][j]<<" ";
                    if(! (maze[i][j]==0 || maze[i][j]==1) )
                    {
                        PRX_FATAL_S("Malformed maze. Cells can either be 0 or 1.");
                    }
                }
            }
            std::cout<<"\n-----------------------------\n";
        }

        void util_application_t::update_visualization()
        {
            PRX_ASSERT(tf_broadcaster != NULL);
            
            //update visualization about where the robot is
            robot_configuration.set_position(_x,_y,_z);
            robot_configuration.set_orientation(_qx,_qy,_qz,_qw);
            goal_configuration.set_position(current_goal_x,current_goal_y,_z);
            goal_configuration.set_orientation(_qx,_qy,_qz,_qw);
            tf_broadcaster->queue_config(robot_configuration, "simulator/disk/ball");
            tf_broadcaster->queue_config(goal_configuration, "simulator/goal/ball");

            
            //send every configuration we have added to the list
            tf_broadcaster->broadcast_configs();
        }

        void util_application_t::info_broadcasting(const ros::TimerEvent& event)
        {
            update_visualization();
            consumed_waypoint = true;
        }

        pluginlib::ClassLoader<util_application_t>& util_application_t::get_loader()
        {
            return loader;
        }


        void util_application_t::execute()
        {
            while(ros::ok())
            {
                if(agent_state == START)
                {
                    PRX_PRINT("Current state is START", PRX_TEXT_BROWN);
                    agent_state = SENSE;
                }
                else if(agent_state == SENSE)
                {
                    PRX_PRINT("Current state is SENSE", PRX_TEXT_BROWN);
                    auto goal = sense();
                    current_goal_i = goal.first;
                    current_goal_j = goal.second;
                    agent_state = PLAN;
                    PRX_PRINT("Sensed: ["<<current_goal_i<<", "<<current_goal_j<<"]", PRX_TEXT_GREEN);
                    auto xy_goal = pose_from_indices(current_goal_i, current_goal_j);
                    current_goal_x = xy_goal.first;
                    current_goal_y = xy_goal.second;
                }
                else if(agent_state == PLAN)
                {
                    PRX_PRINT("Current state is PLAN", PRX_TEXT_BROWN);
                    auto plan_initial = indices_from_pose(_x,_y);
                    PRX_PRINT("Planning from ["<<plan_initial.first<<","<<plan_initial.second<<"] -> ["<<current_goal_i<<","<<current_goal_j<<"]", PRX_TEXT_CYAN);
                    current_path = plan(plan_initial.first, plan_initial.second, current_goal_i, current_goal_j);
                    PRX_PRINT("Path: ", PRX_TEXT_LIGHTGRAY);
                    for(auto p: current_path)
                        std::cout<<"["<<p.first<<","<<p.second<<"]->";
                    std::cout<<"[end]\n";
                    consumed_waypoint = true;
                    path_counter = 0;
                    agent_state = MOVE;
                }
                else if(agent_state == MOVE)
                {
                    if(consumed_waypoint)
                    {
                        PRX_STATUS("Current state is MOVE: ["<<indices_from_pose(_x,_y).first<<", "<<indices_from_pose(_x,_y).second<<"]                ", PRX_TEXT_BROWN);
                        if(path_counter < current_path.size())
                        {
                            move();
                            agent_state = MOVE;
                        }
                        else
                        {
                            agent_state = START;
                        }
                    }
                    else
                    {
                        agent_state = MOVE;
                    }
                }
                ros::spinOnce();
            }
        }


        void util_application_t::move()
        {
            auto current_coordinate = pose_from_indices(current_path[path_counter].first, current_path[path_counter].second);
            _x = current_coordinate.first;
            _y = current_coordinate.second;
            path_counter++;
            consumed_waypoint = false;
        }







        std::pair<int, int> util_application_t::sense()
        {
            //Currently reports a random collision free state as a goal
            while(true)
            {
                int i = uniform_int_random(0,r-1);
                int j = uniform_int_random(0,c-1);
                if(maze[i][j] == 1)
                {
                    return std::make_pair(i,j);
                }
            }
        }

        std::vector< std::pair<int, int> > util_application_t::plan(int initial_i, int initial_j, int goal_i, int goal_j )
        {
            //Input: initial coordinates in maze 2D array: (initial_i, initial_j)
            //       goal coordinates in maze 2D array: (goal_i, goal_j)
            //Return value is a sequence of maze coordinates starting at the initial coordinate and ending at the goal.

            //Returned vector of coordinates.
            std::vector< std::pair<int, int> > path;
            

            // Naive generation of path that has not awareness of the environment
            //################COMMENT OUT THE FOLLOWING BLOCK OF CODE TO POPULATE path##################
            for(int i=initial_i; i<=goal_i; ++i)                                      //################
                path.push_back(std::make_pair(i,initial_j));                          //################            
            for(int i=initial_i; i>=goal_i; --i)                                      //################
                path.push_back(std::make_pair(i,initial_j));                          //################            
            for(int j=initial_j+1; j<=goal_j; ++j)                                    //################
                path.push_back(std::make_pair(goal_i,j));                             //################        
            for(int j=initial_j-1; j>=goal_j; --j)                                    //################
                path.push_back(std::make_pair(goal_i,j));                             //################ 
            //If using C++, you can choose to populate the following function in search.cpp 
            //path = searcher->search();           
            //################THE PRECEDING CODE SHOULD BE REPLACED BY YOUR SOLUTION####################

            //You can invoke your code using an std::system call, or write your code in C++ and include it here, or invoke your code through ROS
            //Global variable environment_file has the path to the maze file
            //###############################################################
            //###############################################################
            //###############################################################
            //###############################################################
            //#########################ASSIGNMENT 1##########################
            //###############################################################
            //###############################################################
            //###############################################################
            //###############################################################



            //Once a path has been reconstructed it is returned
            return path;
        }

    }
}