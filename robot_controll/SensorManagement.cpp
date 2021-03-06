#include "SensorManagement.h"

SensorManagement::SensorManagement() {
    predicted_data = 0;
    laser = new int[RANGE_DATA_COUNT];
}

void SensorManagement::init(int is_online_mode) {
    online_mode = is_online_mode;
    // new VisionContext(); je prazdny kontext
    VisionContext* con = new VisionContext();
    //VisionContext* con = new ContextHistogram();
    // new VisionModifier(); je prazdny modifier
    VisionModifier* mod = new VisionModifier();
    vector<int> vmod;
    vector<int> vcon;
    nn = new VisionBase(5, 5, 5, 5, 1, 1, con, vcon, mod, vmod);
    ed = new EvalDirection((nn->out_width) * 0.4, (nn->out_height) * 0.7, 10,
                           nn->out_width, nn->out_height);
    loc = new LocalizationAndPlaning(800, 400);
    coor = new Coordinate();
    gps = new GpsThread(online_mode);
    imu = new ImuThread(online_mode);
    sbot = new SbotThread(online_mode);
    hokuyo = new HokuyoThread(online_mode);

    BindSerialPorts bs;
    if (online_mode)
      bs.bind(sbot, gps, imu);

    gps->init();
    imu->init();
    hokuyo->init();

    sbot->run();
    gps->run();
    imu->run();
    hokuyo->run();

    sbot->setMaxSpeed(20);
    // nn.load("../plzen4.net");//tiez mozno je ale az moc preuceno
    // nn.load("../555511.net");
    // nn->load("../555511.net");
    // nn->load("../net62.net");
}

int SensorManagement::update() {
    sdata = sbot->getData();
    idata = imu->getData();
    gdata = gps->getData();
    angles = loc->update(gdata);
    hokuyo->getData(laser);
    return 1;
}

int SensorManagement::neuralPredict(IplImage* frame) {
    if (predicted_data != 0)
        cvReleaseMat(&predicted_data);
    predicted_data = nn->predict(frame);
}

void SensorManagement::stop() {
    sbot->stop();
    gps->stop();
    imu->stop();
    hokuyo->stop();
}

int SensorManagement::move() {
    return coor->move(predicted_data, sbot, angles, idata.xAngle, ed,
                      laser);
}

double SensorManagement::evalNeuralForDirection(int dir) {
    return ed->eval(predicted_data, dir);
}

int SensorManagement::getDirNum() { return ed->dir_count; }

IplImage* SensorManagement::getLaserFrame() {
    return hokuyo->getGuiFrame(laser);
}

void SensorManagement::saveHokuyoLog(char* path) {
    FILE* f = fopen(path, "a+");
    if (!f) {
        perror("Cannot open Hokuyo log for appending");
        return;
    }
    for (int i = 0; i < RANGE_DATA_COUNT; i++) {
        fprintf(f, "%d", laser[i]);
        if (i < RANGE_DATA_COUNT - 1)
            fprintf(f, " ");
    }
    fprintf(f, "\n");
    fclose(f);
}
