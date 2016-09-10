
#include "VisionBase.h"
#include "VisionContext.h"
#include "VisionModifier.h"

using namespace std;

//in_x,y je velkost vstupov, out velkost vystupov, in >= out

VisionBase::VisionBase(int step_x, int step_y, int in_x, int in_y, int out_x, int out_y, VisionContext* con,vector<int> vcp, VisionModifier* mod,vector<int> vmp) {
    this->step_x = step_x;
    this->step_y = step_y;
    this->in_x = in_x;
    this->in_y = in_y;
    this->out_x = out_x;
    this->out_y = out_y;
    this->context = con;
    this->modifier = mod;
    //init contexts and mods
    vector<int> pars;
    pars.push_back(step_x);
    pars.push_back(step_y);
    pars.push_back(in_x);
    pars.push_back(in_y);
    pars.push_back(out_x);
    pars.push_back(out_y);
    mod->set_params(vmp,pars);
    con->set_params(vcp,pars);
    channels = 3; //RGB

    this->input_neurons = in_x * in_y * 3;
    this->input_neurons += mod->getSize();
    this->input_neurons += con->getSize();
    
    this->output_neurons = out_x * out_y;

}
//init nn

void VisionBase::init(int hidden_neurons, int hidden_layers) {

    unsigned int ar[hidden_layers + 2];

    ar[0] = this->input_neurons;
    for (int i = 1; i <= hidden_layers; i++) {
        ar[i] = hidden_neurons;
    }
    ar[hidden_layers + 1] = this->output_neurons;


    ann = fann_create_standard_array(sizeof (ar) / sizeof (int), ar);

    //TODO poskusat ine ako FANN_SIGMOID napr gausian sa zda vhodny
    fann_set_activation_function_hidden(ann, FANN_SIGMOID);
    fann_set_activation_function_output(ann, FANN_SIGMOID);

}

//vytvori trenovaci subor z obrazkov rovnomerne zvolenymi oknami

int VisionBase::create_training_file_full(vector<IplImage*> inputs, vector< vector<int> >& outputs, char* out_file) {
    //TODO nejak treshold pridaj nehardkodovo

    FILE* outFile = fopen(out_file, "wt");
    if (outFile == NULL) {
        return 0;
    }
    int counte = 0;
    for (int k = 0; k < inputs.size(); k++) {
        counte += (1 + ((inputs[k]->width - in_y) / step_y))*(1 + (inputs[k]->height - in_y) / step_y);
    }
    fprintf(outFile, "%d %d %d\n",(int) counte, this->input_neurons, this->output_neurons);
    int posi = 0,neg = 0;
    CvScalar s;
    cv::Vec3b tri;
    int pom = 0;
    int res = 0;
    int xo = 0;
    int yo = 0;
    for (int k = 0; k < inputs.size(); k++) {
        //cout<<"creating "<<k+1<<" of "<<inputs.size()<<"\n";
        //vytvor kontext a mod verziu vstupu        
        vector<float> saa = context->getContext(inputs[k]);
        cv::Mat modded = modifier->getContext(inputs[k]);
        for (int x = 0; x <= (inputs[k]->width - in_x); x += step_x) {
            for (int y = 0; y <= (inputs[k]->height - in_y); y += step_y) {
                //vypis vstup
                for (int i = 0; i < in_x; i++) {
                    for (int j = 0; j < in_y; j++) {
                        //TODO get pixel value sa da aj efektivnejsie
                        s = cvGet2D(inputs[k], y + j, x + i); //get2D ma druhy parameter y a treti x
                        for (int c = 0; c < channels; c++) {
                            fprintf(outFile, "%f ", (float)s.val[c]/(float)255);
                        }
                        //mod
                        tri = modded.at<cv::Vec3b>(y + j, x + i);
                        for (int c = 0; c < modded.channels(); c++) {
                            fprintf(outFile, "%f ", (float)tri.val[c]/(float)255);
                        }
                    }
                }
                //context
                for(int rr = 0; rr < saa.size(); rr++){
                    fprintf(outFile, "%f ", saa[rr]);
                }
                
                fprintf(outFile, "\n");

                //vypis chceny vystup

                xo = x + in_x / 2 - step_x / 2;
                yo = y + in_y / 2 - step_y / 2;
                //kazdy neuron
                for (int f = 0; f < out_x; f++) {
                    for (int g = 0; g < out_y; g++) {
                        pom = 0;
                        //vsetky pixely neurona
                        for (int d = xo + f * (step_x / out_x); d < xo + (f + 1)*(step_x / out_x); d++) {
                            for (int e = yo + g * (step_y / out_y); e < yo + (g + 1)*(step_y / out_y); e++) {
                              
                                if (outputs[k][inputs[k]->width*e + d] >0) {
                                    pom++;
                                }
                            }
                        }
                        res = 0;
                        //ak je pomer viac ako 50% neuron ma svietit TODO nech neni nahardkodene takto
                        if (((float)pom / ((step_x / out_x) * (step_y / out_y))) >= 0.5) {
                            res = 1;
                        }
                        if(res == 1)posi++;else neg++;
                        fprintf(outFile, "%d ", res);
                        

                    }
                }

                fprintf(outFile, "\n");

            }
        }
    }
    cout<< "train pos" << posi << " neg"  <<neg<<"\n";
    fclose(outFile);
    return 1;
}
//vytvori trenovaci subor z obrazkov nahodne zvolenymi oknami
//TODO urob na vector ako vo full
int VisionBase::create_training_file_partial(vector<IplImage*> inputs, vector< vector<int> >& outputs, char* out_file, int samples) {

    FILE* outFile = fopen(out_file, "wt");
    if (outFile == NULL) {
        return 0;
    }

    fprintf(outFile, "%d %d %d\n",(int) inputs.size() * samples, this->input_neurons, this->output_neurons);

    CvScalar s;
    int pom = 0;
    int res = 0;
    int x = 0;
    int y = 0;
    int xo = 0;
    int yo = 0;
    for (int k = 0; k < inputs.size(); k++) {

        for (int rnc = 0; rnc < samples; rnc++) {
            //get random x, y
            x = (((double) rand() / (double) RAND_MAX)* (inputs[k]->width - in_x - 1));
            y = (((double) rand() / (double) RAND_MAX)* (inputs[k]->height - in_y - 1));
            //vypis vstup
            for (int i = 0; i < in_x; i++) {
                for (int j = 0; j < in_y; j++) {
                    //TODO get pixel value sa da aj efektivnejsie
                    s = cvGet2D(inputs[k], y + j, x + i); //get2D ma druhy parameter y a treti x
                    for (int c = 0; c < channels; c++) {
                        fprintf(outFile, "%f ", s.val[c]);
                    }
                }
            }
            fprintf(outFile, "\n");

            //vypis chceny vystup

            xo = x + in_x / 2 - step_x / 2;
            yo = y + in_y / 2 - step_y / 2;
            
            //kazdy neuron
            for (int f = 0; f < out_x; f++) {
                for (int g = 0; g < out_y; g++) {
                    pom = 0;
                    //vsetky pixely neurona
                    for (int d = xo + f * (step_x / out_x); d < xo + (f + 1)*(step_x / out_x); d++) {
                        for (int e = yo + g * (step_y / out_y); e < yo + (g + 1)*(step_y / out_y); e++) {
                            //TODO nepassuj cvmat ale len vector a getni ho width*e +d
                            
                            if (outputs[k][inputs[k]->width*e + d]>0) {
                                pom++;
                            }
                        }
                    }
                    res = 0;
                    //ak je pomer viac ako 50% neuron ma svietit treshold
                    if ((pom / (step_x / out_x * step_y / out_y)) >= 0.5) {
                        res = 1;
                    }
                    fprintf(outFile, "%d ", res);

                }
            }

            fprintf(outFile, "\n");


        }
    }
    fclose(outFile);
    return 1;
}

//train on training file

double VisionBase::train(const float desired_error, const unsigned int max_epochs, char* train_file) {
    const unsigned int epochs_between_reports = 5;
    fann_reset_MSE(ann);

    fann_train_on_file(ann, train_file, max_epochs, epochs_between_reports, desired_error);

    return fann_get_MSE(ann);
}

double VisionBase::train_on_data(const float desired_error, const unsigned int max_epochs, fann_train_data* data) {
    const unsigned int epochs_between_reports = 5;
    fann_reset_MSE(ann);
    fann_train_on_data(ann, data, max_epochs, epochs_between_reports, desired_error);

    return fann_get_MSE(ann);
}

//test returns MSE of data

double VisionBase::test(char* train_file) {

    fann_reset_MSE(ann);

    fann_train_data* data = fann_read_train_from_file(train_file);

    fann_test_data(ann, data);

    fann_destroy_train(data);

    return fann_get_MSE(ann);
}

double VisionBase::test_on_data(fann_train_data* data) {

    fann_reset_MSE(ann);

    fann_test_data(ann, data);

    fann_destroy_train(data);

    return fann_get_MSE(ann);
}


//process image with nn

vector<fann_type> VisionBase::predict(IplImage* inputs) {

    vector<fann_type*> input_array;
    vector<fann_type> calc_out;
    int pom = 0;int kk=0;
    CvScalar s;
    cv::Vec3b tri;
    vector<float> saa = context->getContext(inputs);
    cv::Mat modded = modifier->getContext(inputs);
    
    for (int y = 0; y <= (inputs->height - in_y); y += step_y) {
        for (int x = 0; x <= (inputs->width - in_x); x += step_x) {
            fann_type* row = new fann_type[ ann->num_input ];

            pom = 0; //pomocny iterator nech je menej instrukcii

            for (int i = 0; i < in_x; i++) {
                for (int j = 0; j < in_y; j++) {
                    //TODO get pixel value sa da aj efektivnejsie
                    s = cvGet2D(inputs, y + j, x + i); //get2D ma druhy parameter y a treti x
                    for (int c = 0; c < channels; c++) {
                        row[ pom ] = (float)s.val[c]/(float)255;
                        pom++;
                    }
                    //mod
                    /* TODO check if matrix exists
                    tri = modded.at<cv::Vec3b>(y + j, x + i);
                    for (int c = 0; c < modded.channels(); c++) {
                        row[ pom ] = (float)tri.val[c]/(float)255;
                        pom++;
                    }*/
                }
            }
            for(int rr = 0; rr < saa.size(); rr++){
                    row[pom] = saa[rr];
                    pom++;
                }
            input_array.push_back(row);
        }
    }
    cout<<"\n"<< kk<<" input sajz "<<input_array.size()<<"\n";
    //TODO vytvor vystup  run vracia array tu je len s 1 out neuronom p[0] inak by bolo p[0 - #of neurons]
    //TODO urob to bez input array len v 1 cykle volat hned run takto zbytocne plnim array
    
    vector<fann_type*>::iterator it;
    for (it = input_array.begin(); it != input_array.end(); it++) {

        fann_scale_input(ann, (*it)); //TODO scale to [1,-1] by handwritten function -better lebo fann to robi nespecificky pre problem
        fann_type* p = fann_run(ann, (*it));
        for(int u = 0; u < output_neurons; u++)
            calc_out.push_back(p[u]);

        delete[] (*it);
    }

    //for(int u = 0; u < input_array.size(); u++)
     //   delete input_array[u];
    input_array.clear();
    

    return calc_out;
}

//upscalne result tak aby sa dal pouzit ako overlay obrazku
//TODO nejak efektivnejsie sa neda? cez copy alebo nejake memcpy...
 vector<int>  VisionBase::upscale_result(vector<fann_type> v,int image_width, int image_height){
    cout<< "upscale "<<v.size()<<"\n";
    vector<int>  res;
    vector<int>  row;
    int itr = 0;
    bool margin = true;
    if(margin){
        //na zaciatok prazdne riadky ktore sa nehodnotili
        for(int s= 0; s< (in_y - step_y)/2; s++){
            for(int z = 0; z< image_width;z++)
                res.push_back(0);        
        }
    } 
    for (int y = 0; y <= (image_height - in_y); y += step_y) {
        if(margin){
            //zaciatok rowu
            for(int s= 0; s< (in_x - step_x)/2; s++){
                row.push_back(0);
            }
        }
        for (int x = 0; x <= (image_width - in_x); x += step_x) {  
            itr++;
            for(int h = 0; h < step_x; h++){
                if(v[itr] > 0.5)
                    row.push_back(1);
                else
                    row.push_back(0);
            }
            
        }
        if(margin){
            //koniec rowu
            for(int s= 0; s< (in_x - step_x) - (int)((in_x - step_x)/2); s++){
                row.push_back(0);
            }
        }
        for(int h = 0; h < step_y; h++){
                for(int k = 0; k < row.size();k++){
                    res.push_back(row[k]);
                }
        }
        row.clear();
    }
    if(margin){
        //na koniec riadky neohodnotene
        for(int s= 0; s< (in_y - step_y) -(int)((in_y - step_y)/2); s++){
            for(int z = 0; z< image_width;z++)
                res.push_back(0);        
        }
    }
    cout<< "upscaled to "<<res.size()<<"\n";
    return res;
   
}

void VisionBase::load(const char* filepath) {
    ann = fann_create_from_file(filepath);
}

void VisionBase::save(const char* filepath) {
    fann_save(ann, filepath);
}

void VisionBase::close() {
    fann_destroy(ann);
}

void VisionBase::scale_data(char* file, char* out_file){
    
    fann_train_data* data = fann_read_train_from_file( file );

    fann_set_scaling_params( ann, data, 0.0f, 1.0f, 0.0f, 1.0f );

    fann_scale_train(ann, data);

    fann_save_train(data, out_file);
}