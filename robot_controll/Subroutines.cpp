#include "Subroutines.h"

Subroutines::Subroutines() {
	was_obstacle = false;
	setstart = 0;
	finishcount = 0;
	found_obstacle = 0;
	finnumber = 0;
	
}

void Subroutines::testSbot()
{
    sleep(1);
    sbot->setSpeed(1);
    sleep(1);
    sbot->setSpeed(0);
}

void Subroutines::setup(LocalizationAndPlaning* lc, SbotThread* sb)
{
    loc = lc;
    sbot = sb;
}

void Subroutines::manageObstacles(SensorManagement sm)
{
	if (sm.sdata.obstacle && (!was_obstacle))
	{
		time_t tobs = time(NULL);
		found_obstacle = tobs;
		printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!OBSTACLE\n");
                sm.coor->move_status = "obstacle";
                
	}
	time_t tmnow = time(NULL);
	if ((sm.sdata.obstacle) && (tmnow - found_obstacle > 20))
	{
		printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! BACKING UP\n");
		found_obstacle = found_obstacle +30;
		// cuvat alebo aspon tocit!!
		double lft = sm.evalNeuralForDirection( 0 );
		double rgh = sm.evalNeuralForDirection( sm.getDirNum() );
		int bakdir = 0;
		if(lft>rgh){
			bakdir = -40;
			printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! LEFT\n");
                        sm.coor->move_status = "back up left";
		}
		else{
			bakdir = 40;
			printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! RIGHT\n");
                        sm.coor->move_status = "back up right";
		}
		sbot->ignoreObstacle(true);
		sbot->setDirection( 0 );
		sbot->setSpeed( -3 );
		sleep(4);
		sbot->setDirection( bakdir );
		sbot->setSpeed( 2 );
		sleep(2);
		sbot->setDirection( 0 );                                                                                       
		sbot->setSpeed( 0 );
		sleep(2);
		sbot->ignoreObstacle(false);
	}
	else if (sm.sdata.obstacle)
	{
		printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! %ld\n", tmnow - found_obstacle);
		fflush(stdout);
                char str[20];
                sprintf(str, "obstacle %ld", tmnow - found_obstacle);
                sm.coor->move_status = str;
	}
	was_obstacle = sm.sdata.obstacle;
        sm.coor->status_from_subroutines = sm.sdata.obstacle;
}

int Subroutines::manageFinish(SensorManagement sm)
{
	if(setstart == 0){
		setstart = 1;
		startpos = sm.gdata;
	}
	
	//TODO ked 10sec stoji ze je v cieli vypni ho uz(ak je uspesne to zastavenie)
	if(sm.angles.map ==DBL_MAX){
		finishcount++;
	}
	else{
		finishcount = 0;
	}
	if(finishcount>60){
		if(finnumber == 2){
			printf("FINISH CONFIRMED with %d readings , TURNING OFF\n",finishcount);
                        sm.coor->move_status = "finish - turn off";
                        sm.coor->status_from_subroutines = true;
                        
			return true;
		}
		else{
			for(int ghe = 0 ; ghe <7; ghe++)
					printf("FINISH REACHED GOING BACK TO START\n");
                        sm.coor->move_status = "finish - go back";
                        sm.coor->status_from_subroutines = true;
			//TODO set next dest
			sm.loc->setDestination(startpos);
                        sm.loc->bestWay.clear();
			finnumber = 2;
		}
	}
        sm.coor->status_from_subroutines = false;
	return false;
}

