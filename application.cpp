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

// Includes for python-io
#include <netinet/in.h>
#include <sys/wait.h>

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

        std::vector< std::pair<int, int> > util_application_t::plan(int initial_i, int initial_j, int goal_i, int goal_j ) {

            std::vector< std::pair<int,int> > path;

            int server_fd, new_socket;
            struct sockaddr_in address;
            int addrlen = sizeof(address);
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = INADDR_ANY;
            address.sin_port = 0;
            
            // Creating socket file descriptor
            if (!(server_fd = socket(AF_INET, SOCK_STREAM, 0))) {
                perror("error creating socket");
                exit(EXIT_FAILURE);
            }
            
            // Bind socket to random port
            if (bind(server_fd, (struct sockaddr *) &address, addrlen)) {
                perror("error binding to random port");
                exit(EXIT_FAILURE);
            }

            // Save randomly selected port
            if (getsockname(server_fd, (struct sockaddr *) &address, (socklen_t *) &addrlen)) {
                perror("error saving random port");
                exit(EXIT_FAILURE);
            }
            int port = ntohs(address.sin_port);
            
            if(fork()) {

                // Listen on socket
                if (listen(server_fd, 1) < 0) {
                    perror("error listening to port");
                    exit(EXIT_FAILURE);
                }

                // Accept socket connection
                if ((new_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t *) &addrlen)) < 0) {
                    perror("error accepting connection");
                    exit(EXIT_FAILURE);
                }

                // Get number of pairs
                unsigned int numPairs;
                read(new_socket, &numPairs, 4);
                numPairs = ntohl(numPairs);

                // Fill path vector
                for (unsigned int i = 0; i < numPairs; i++) {
                    std::pair<int,int> tuple;
                    read(new_socket, &tuple.first, 4);
                    read(new_socket, &tuple.second, 4);
                    tuple.first = ntohl(tuple.first);
                    tuple.second = ntohl(tuple.second);
                    path.push_back(tuple);
                }

                wait(NULL);

            } else {

                // Create argument strings
                char mainPyPath[PATH_MAX];
                sprintf(mainPyPath, "%s/python/cpp-io.py", getenv("PRACSYS_PATH"));
                char portStr[10];
                sprintf(portStr, "%d", port);
                char iiStr[10];
                sprintf(iiStr, "%d", initial_i);
                char ijStr[10];
                sprintf(ijStr, "%d", initial_j);
                char giStr[10];
                sprintf(giStr, "%d", goal_i);
                char gjStr[10];
                sprintf(gjStr, "%d", goal_j);

                // Run python script
                execl(
                    "/usr/bin/python3",
                    "python3",
                    mainPyPath,
                    portStr,
                    environment_file,
                    iiStr,
                    ijStr,
                    giStr,
                    gjStr,
                    NULL
                );
            }
            
            return path;
        }
    }
}
