Software repository for mobile robot Smely Zajko.
Visit project homepage: http://dai.fmph.uniba.sk/projects/smelyzajko

(c) Faculty of Mathematics, Physics, and Informatics, Comenius University, Bratislava, Slovakia.


repository structure:

controlboard/ - Atmel AVR Studio 4 project for main robot control board based on ATmega128
robot_controll/ - the main application running on the linux with ubuntu linux
nn_lib/ - the library for running a neural network
nn_train/ - the application to train a neural network
gen_train_data/ - the application to generate the training data - unfortunately built using some outdated QT library (needs refreshing)
TrainingCrt/ - the application to create the trainig data

destination.txt - contains GPS coordinates of the robot destination location
*.net - a trained neural network

postman/ - contains application for postman delivery project
