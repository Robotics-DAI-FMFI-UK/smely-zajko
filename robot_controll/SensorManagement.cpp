#include "SensorManagement.h"

SensorManagement::SensorManagement() 
{
    predicted_data = 0;
    laser = new int[RANGE_DATA_COUNT];
}

void SensorManagement::init()
{
    VisionContext* con = new VisionContext();//new VisionContext(); je prazdny kontext
    VisionModifier* mod = new VisionModifier(); //new VisionModifier(); je prazdny modifier
    vector<int> vmod;
    vector<int> vcon;
    nn = new VisionBase(5, 5, 5, 5, 1, 1,con,vcon,mod,vmod);
    ed = new EvalDireciton( (nn->out_width)*0.4, (nn->out_height)*0.7, 10, nn->out_width, nn->out_height);
    loc = new LocalizationAndPlaning(800,400);
    coor = new Coordinate();
    gps = new GpsThread();
    imu = new ImuThread();
    sbot = new SbotThread();
    hokuyo = new HokuyoThread();
    	
    BindSerialPorts bs;
    bs.bind( sbot, gps, imu );
	
    gps->init();
    imu->init();
    hokuyo->init();
     
    sbot->run();
    gps->run();
    imu->run();
    hokuyo->run();
   
	
	//nn.load("../plzen4.net");//tiez mozno je ale az moc preuceno
    //nn.load("../555511.net");
    nn->load("../555511.net");
    //nn->load("../net62.net");
}

int SensorManagement::update()
{
    sdata = sbot->getData();
    idata = imu->getData();
    gdata = gps->getData();
    angles = loc->update(gdata);
    hokuyo->getData(laser);
    return 1;
}

int SensorManagement::neuralPredict(IplImage* frame)
{
   if (predicted_data != 0) cvReleaseMat( &predicted_data );
   predicted_data = nn->predict( frame );
}

void SensorManagement::stop()
{
    sbot->stop();
    gps->stop();
    imu->stop();
    hokuyo->stop();
}

int SensorManagement::move()
{ 
    return coor->move(predicted_data, sbot, angles.map, idata.xAngle, ed, laser );
}

double SensorManagement::evalNeuralForDirection(int dir)
{
	return ed->eval(predicted_data, dir);
}

int SensorManagement::getDirNum()
{
	return ed->dir_count;
}

IplImage* SensorManagement::getLaserFrame()
{
    return hokuyo->getGuiFrame(laser);
}

void SensorManagement::saveHokuyoLog(char* path)
{
    FILE *f = fopen(path, "a+");
    if (!f)
    {
        perror("Cannot open Hokuyo log for appending");
        return;
    }
    for (int i = 0; i < RANGE_DATA_COUNT; i++)
    {
      fprintf(f, "%d", laser[i]);
      if (i < RANGE_DATA_COUNT - 1) fprintf(f, " ");
    }
    fprintf(f, "\n");
    fclose(f);    
}