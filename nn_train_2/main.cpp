/* 
 * File:   main.cpp
 * Author: mnam
 *
 * Trenovanie siete - 
 */

#include <cstdlib>
#include <algorithm> 
#include <dirent.h>
#include <fstream>
#include "VisionBase.h"
#include "ContextHistogram.h"
#include "ContextProbableRectangle.h"
#include "ContextHistPlusRect.h"
#include "ModKMeans.h"
#include <opencv2/opencv.hpp>
#include <fann.h>
#include <memory>
#include <unistd.h>
using namespace std;

/*
 * 
 */

vector<int> getvector(string s) {
    vector<int> resu;
    // open file    
    ifstream inputFile(s.c_str());

    // test file open   
    if (inputFile) {
        int value;

        // read the elements in the file into a vector  
        while (inputFile >> value) {
            resu.push_back(value);
        }
    }
    return resu;
}
//da riadok do logu podla parametrov
void logminor(double validacnachyba, double najMSE, vector<int> params){
    char file[64];
    sprintf(file, "output/model_%d_%d_%d_%d_%d_%d_%d_%dlog.log", params[0], params[1], params[2], params[3], params[4], params[5], params[6],params[7]);
    ofstream outfile;
    outfile.open(file, std::ios_base::app);
    
    outfile << "validacna chyba " << "najMSE \n";
    outfile << validacnachyba << " " << najMSE<<"\n";
}
//da riadok do hlavneho logu
void logmajor(double priemernavalidacna,double najvalidacna, vector<int> params){
    char file[64];
    sprintf(file, "output/mainlog.log");
    ofstream outfile;
    outfile.open(file, std::ios_base::app);
    
    outfile << params[0] << "_" << params[1] <<"_"<< params[2] <<"_"<< params[3] <<"_"<< params[4] <<"_"<< params[5] <<"_"<< params[6] <<"_"<< params[7] ;
    outfile << " -> "<< priemernavalidacna<<" " << najvalidacna<<"\n";
    
}

vector<string> getfiles(string dir) {
    vector<string> files;
    
    if (dir.substr(dir.length() - 1, 1).compare("/") != 0) {
        dir += "/";
    }
    DIR *dp;
    struct dirent *dirp;
    if ((dp = opendir(dir.c_str())) == NULL) {
        printf("error cannot open directory\n");
        return files;
    }
    

    while ((dirp = readdir(dp)) != NULL) {
        files.push_back(string(dirp->d_name));
    }
    return files;
}

void getInputOutputVectors(vector<string> &files,string dir,vector<IplImage*> &inputs, vector< vector<int> > &outputs){
    for (int i = 0; i < files.size(); i++) {
        if (files.at(i).length() > 4) {
            string suffix = files.at(i).substr(files.at(i).length() - 4, 4);
            transform(suffix.begin(), suffix.end(), suffix.begin(), ::tolower);
            if (suffix.compare(".jpg") == 0 || suffix.compare(".png") == 0 || suffix.compare(".bmp") == 0) {
                string path(dir);
                path += files.at(i);

                //output
                string out_path = path + ".trn";

                if (access( out_path.c_str(), F_OK | R_OK ) == -1 ){
                    cout << "output " << out_path << " is not accessible\n";
                    cout << access( out_path.c_str(), F_OK | R_OK );
                    cout << access( path.c_str(), F_OK | R_OK );
                    continue;
                }
                    
                
                outputs.push_back(getvector(out_path));
                
                IplImage* tmp = cvLoadImage((const char *) path.c_str());
                                
                inputs.push_back(cvCreateImage(cvSize(tmp->width, tmp->height), tmp->depth, tmp->nChannels));
                //lab conversion
                cvCvtColor(tmp, inputs.back(), CV_BGR2Lab);
                //rgb
                //cvResize( tmp, inputs.back() );
                cvReleaseImage(&tmp);


            }
        }
    }
    cout << outputs.size() << "found images to train on" << "\n";
}

void shuffle(vector<IplImage*> &x, vector< vector<int> > &y){
    
    int j;
    srand(time(NULL));
    for(int i = x.size()-1; i>0; i--){
            //get fairly random num from 0 to i not uniform dist but we dont care about it in this case..
           j = rand() %(i+1);
           swap(x[i],x[j]);
           swap(y[i],y[j]);
    }
    
}

int main(int argc, char** argv) {
    
    
    printf("start\n");

    //load input and corresponding output files into array
    string dir("parky/");//z nejakeho dovodu nevie ist von zo svojho adresara a mat pristup..
    //string dir("test/");
    vector<string> files = getfiles(dir);
    if (files.empty()) {
        printf("No train files found\n");
        return -2;
    }
    //return 0;
    vector<IplImage*> inputs;
    vector< vector<int> > outputs;
    getInputOutputVectors(files, dir, inputs, outputs);
    shuffle(inputs, outputs);

    //testovanie
    /* 
    VisionContext* con1 = new ContextProbableRectangle();//new VisionContext();
    VisionModifier* mod1= new VisionModifier(); //new ModKMeans();
    vector<int> vmod1;
    vector<int> vcon1 = {8,10,1};
    
    auto_ptr<VisionBase> nn2 (new VisionBase( 5,5,5,5,1,1,con1,vcon1,mod1,vmod1 ));
            nn2->init(8,1);
           
    char net_name[64];
    sprintf(net_name, "NAJNEUR.net");
    nn2->load(net_name);
    
    for(int hura = 0; hura < inputs.size();hura++){
        //try predict and save result
        printf("saveaae\n");
        vector<float> reza = nn2->predict(inputs[hura]);
        for(int yy = 0; yy < reza.size(); yy++)
            cout<<reza[yy]<<" ";
        printf("bbbbbb\n");
        vector<int> rez = nn2->upscale_result(reza,inputs[hura]->width,inputs[hura]->height); 
        printf("savee\n");
        //save rez
        char save_name[64];
        sprintf(save_name, "save%d.trn",hura);
        FILE* outFile = fopen(save_name, "wt");
        if (outFile == NULL) {
            return 0;
        }

        for(int zo = 0; zo < rez.size();zo++)
            fprintf(outFile, "%d ", rez[zo]);
        fprintf(outFile, "\n");
        fclose(outFile);
    }
    return 0;
    
    */
    
    
    
    
    
    
    
    //divide for k cross validation
    int k = 10;
    
    vector< vector<IplImage*> > kcrsin(k);
    vector< vector< vector<int> > > kcrsout(k);
    int curK = 0;
    for (int i = 0; i < inputs.size(); i++) {
        kcrsin[curK].push_back(inputs[i]);
        kcrsout[curK].push_back(outputs[i]);
        curK++;
        if (curK >= k) {
            curK = 0;
        }
    }

    //init for train files

    char filename2[64];
    sprintf(filename2, "train%d.data", 666);
    char filename[64];
    sprintf(filename, "sctrain%d.data", 666);

    char filename4[64];
    sprintf(filename4, "test%d.data", 666);
    char filename3[64];
    sprintf(filename3, "sctest%d.data", 666);
    
    vector<IplImage*> validin;
    vector< vector<int> > validout;

    vector<IplImage*> trnin;
    vector< vector<int> > trnout;

    vector<int> params;
    
    /*
    
    ContextHistogram tstzmaz;
    tstzmaz.set_params({10},{20});
    vector<float> iia = tstzmaz.getContext(inputs[3]);
    cout<<iia.size()<<"\n";
    for(int g = 0; g < iia.size();g++)
        cout<<iia[g]<<"\n";
    */
    /*
    VisionContext con;
    VisionModifier mod;
    vector<int> vmod;
    vector<int> vcon;
    */
    VisionContext* con = new ContextHistPlusRect();//new VisionContext();
    VisionModifier* mod = new VisionModifier(); //new ModKMeans();
    vector<int> vmod;
    vector<int> vcon;// = {8,10,1};
    //try different settings and do k-cross validation
    vector< vector<int> > par;
    //par.push_back({5,5,5,5,1,1,8,1});
    //par.push_back({5,5,5,5,1,1,12,1});
    //par.push_back({5,5,5,5,1,1,20,1});
    par.push_back({5,5,5,5,1,1,6,2});
    par.push_back({5,5,5,5,1,1,10,2});
    printf("cross valid started\n");
    for(int p = 0; p < par.size(); p++){
        params.clear();
        params = par[p];
        
        printf("current model : \n");
        printf("model_%d_%d_%d_%d_%d_%d_%d_%d\n", params[0], params[1], params[2], params[3], params[4], params[5], params[6],params[7]);
        
        double bestvalid = DBL_MAX;
        double sumvalid = 0;
        //ukladanie najlepsiej s danym modelom
        char pathzliabok[64];        
        sprintf(pathzliabok, "output/model_%d_%d_%d_%d_%d_%d_%d_%dneuronka.net", params[0], params[1],params[2],params[3],params[4],params[5],params[6],params[7]);
        //ukladanie docasne dobrej nn
        char pathzliabokdocas[64];
        sprintf(pathzliabokdocas, "output/neuronkadocas.net");
        
        //for(int kit = 0; kit < k; kit++){ nieje cas na kompletny kcross
        for(int kit = 0; kit < 3; kit++){
            
            //init
            auto_ptr<VisionBase> nn (new VisionBase( params[0], params[1], params[2], params[3], params[4], params[5],con,vcon,mod,vmod ));
            nn->init(params[6],params[7]);
            printf("init nn podareny k=%d \n",kit);
            //create traindata and testdata
            //narvi tych k-1 do train a 1 ako valid
            for(int ac = 0; ac < k; ac++){
                if(ac == kit){
                    for(int i = 0; i<kcrsin[ac].size(); i++){
                        validin.push_back(kcrsin[ac][i]);
                        validout.push_back(kcrsout[ac][i]);
                    }
                }
                else{
                    for(int i = 0; i<kcrsin[ac].size(); i++){
                        trnin.push_back(kcrsin[ac][i]);
                        trnout.push_back(kcrsout[ac][i]);
                    }
                }
            }
            //train
            nn->create_training_file_full(trnin, trnout, filename2);
            nn->scale_data(filename2,filename);
            //test
            nn->create_training_file_full(validin, validout, filename4);
            nn->scale_data(filename4,filename3);
            //kcrsin[kit] is validation vector
            
            //load file to train/test data so it does not have to read them every epoch            
            fann_train_data* traindata = fann_read_train_from_file(filename);
            //fann_train_data* testdata = fann_read_train_from_file(filename3);
            cout<< "train input num" << fann_length_train_data(traindata);
            
            //train loop
            double curer = nn->train_on_data(0,1,traindata);
            double beste = curer+1;
            int epc=1;
            int breakearly = 0;
            while(epc < 40){       //40 
                printf("EPOCHA %d z %d \n",(epc-1)*5,40*5);
                if(beste > curer){
                    nn->save(pathzliabokdocas);
                    beste = curer;                
                    breakearly = 0;
                }
                curer = nn->train_on_data(0,5,traindata);        
                epc++;
                breakearly++;
                if(breakearly >7){
                    printf("dlho bez zlepsenia breaking %d \n",breakearly);
                    break;
                }
            }
            nn->load(pathzliabokdocas);
            double e = nn->test(filename3);
            sumvalid +=e;
            if(e < bestvalid){
                bestvalid = e;
                nn->save(pathzliabok);
            }
            nn->close();
            logminor(e,beste,params);//do zliabka podla parametrov//TODO
            validin.clear();
            validout.clear();
            trnin.clear();
            trnout.clear();
        }
        //nieje cas na kompletny kcross
        //logmajor(sumvalid/k,bestvalid,params);//do zliabka hlavneho agregovane vysledky//TODO
        logmajor(sumvalid/3,bestvalid,params);//do zliabka hlavneho agregovane vysledky//TODO
    }
    return 0;
}

/*
 SPAM KODU CO ESTE NEZAHADZUJU
 ModKMeans tstzmaz;
    tstzmaz.set_params({8,5},{20});
    cv::Mat iialice = tstzmaz.getContext(inputs[3]);
    cvNamedWindow("localization", CV_WINDOW_FREERATIO);
    // Display image in window
    //cvSmooth( inputs[3], inputs[3], CV_BLUR, 7, 7 );
    cvShowImage("localization", inputs[3]);
    cv::imshow("foobar", iialice);
    cvWaitKey(0);
 * 
 * 
 * 
 * int xl =0;
    int ml =255;
    int xa =0;
    int ma =255;
    int xb =0;
    int mb =255;
    CvScalar s;
    for(int m = 0; m < inputs.size();m++){
    for (int y = 0; y < inputs[m]->height; y += 1) {
        for (int x = 0; x < inputs[m]->width; x += 1) {
            s = cvGet2D(inputs[m], y , x );
            if(s.val[0] >xl)
                xl = s.val[0];
            if(s.val[1] >xa)
                xa = s.val[1];
            if(s.val[2] >xb)
                xb = s.val[2];
            
            if(s.val[0] <ml)
                ml = s.val[0];
            if(s.val[1] <ma)
                ma = s.val[1];
            if(s.val[2] <mb)
                mb = s.val[2];
        }
    }
    }
    cout << "L:"<<ml<<" az "<<xl<<"\n";
    cout << "A:"<<ma<<" az "<<xa<<"\n";
    cout << "B:"<<mb<<" az "<<xb<<"\n";
 * 
 * 
 */