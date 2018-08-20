echo 'Starting report server...'
gnome-terminal -e "python3 $PRACSYS_PATH/python/report_server.py 8 10 $PRACSYS_PATH/report.txt"

echo 'Backing up original config...'
cp $PRACSYS_PATH/python/config.py $PRACSYS_PATH/python/config.py.original

echo 'Running mazes...'
cp $PRACSYS_PATH/python/report_files/config_files/1 $PRACSYS_PATH/python/config.py
roslaunch prx_core assignment_1.launch env:="$PRACSYS_PATH/python/report_files/mazes/maze10.txt"
roslaunch prx_core assignment_1.launch env:="$PRACSYS_PATH/python/report_files/mazes/maze20.txt"
cp $PRACSYS_PATH/python/report_files/config_files/2 $PRACSYS_PATH/python/config.py
roslaunch prx_core assignment_1.launch env:="$PRACSYS_PATH/python/report_files/mazes/maze10.txt"
roslaunch prx_core assignment_1.launch env:="$PRACSYS_PATH/python/report_files/mazes/maze20.txt"
cp $PRACSYS_PATH/python/report_files/config_files/3 $PRACSYS_PATH/python/config.py
roslaunch prx_core assignment_1.launch env:="$PRACSYS_PATH/python/report_files/mazes/maze10.txt"
roslaunch prx_core assignment_1.launch env:="$PRACSYS_PATH/python/report_files/mazes/maze20.txt"
cp $PRACSYS_PATH/python/report_files/config_files/4 $PRACSYS_PATH/python/config.py
roslaunch prx_core assignment_1.launch env:="$PRACSYS_PATH/python/report_files/mazes/maze10.txt"
roslaunch prx_core assignment_1.launch env:="$PRACSYS_PATH/python/report_files/mazes/maze20.txt"

echo 'Restoring original config...'
cp $PRACSYS_PATH/python/config.py.original $PRACSYS_PATH/python/config.py
rm $PRACSYS_PATH/python/config.py.original
