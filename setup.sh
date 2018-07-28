echo 'Copying c++ file...'
cp application.cpp $PRACSYS_PATH/prx_core/prx/utilities/applications

echo 'Copying python files...'
cp -r python $PRACSYS_PATH

echo 'Compiling...'
cd $PRACSYS_PATH/.. && catkin_make

echo 'Done'
