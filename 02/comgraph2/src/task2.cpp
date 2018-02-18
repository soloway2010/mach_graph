#include <string>
#include <vector>
#include <fstream>
#include <cassert>
#include <iostream>
#include <cmath>

#include "classifier.h"
#include "EasyBMP.h"
#include "matrix.h"
#include "linear.h"
#include "argvparser.h"

using std::string;
using std::vector;
using std::ifstream;
using std::ofstream;
using std::pair;
using std::make_pair;
using std::cout;
using std::cerr;
using std::endl;

using CommandLineProcessing::ArgvParser;

typedef vector<pair<BMP*, int> > TDataSet;
typedef vector<pair<string, int> > TFileList;
typedef vector<pair<vector<float>, int> > TFeatures;

const int SEGS = 8;
const int DIRS = 16;
const double Pi = 3.14;

//My functions
void toGreyScale(BMP* image, Matrix<double>& grey_image){

	 for(int i = 0; i < image->TellHeight(); i++)
        	for(int j = 0; j < image->TellWidth(); j++){

        		RGBApixel rgb_pixel = image->GetPixel(j, i);

       			grey_image(i, j) = 0.299*rgb_pixel.Red + 0.587*rgb_pixel.Green + 0.114*rgb_pixel.Blue;
    		}
}

void getGrad(Matrix<double>& image, Matrix<double>& grad_image, Matrix<double>& dir_image){

	for(uint i = 1; i < image.n_rows - 1; i++)
		for(uint j = 1; j < image.n_cols - 1; j++){
			double x_dif = -image(i - 1, j - 1) + image(i + 1, j - 1)
						   -image(i - 1, j)     + image(i + 1, j)
						   -image(i - 1, j + 1) + image(i + 1, j + 1);

			double y_dif = -image(i - 1, j - 1) + image(i - 1, j + 1)
						   -image(i, j - 1)     + image(i, j + 1)
						   -image(i + 1, j - 1) + image(i + 1, j + 1);

			grad_image(i, j) = sqrt(x_dif*x_dif + y_dif*y_dif);
			dir_image(i, j) = atan2(y_dif, x_dif);
		}
}

int getDir(double angle){
	for(int i = 0; i < DIRS; i++)
		if(angle >= -Pi + i*2*Pi/DIRS && angle < -Pi + (i + 1)*2*Pi/DIRS)
			return i;

	return 0;
}

void LBP(Matrix<double>& image, vector<float>& vec){
	float hist[256 * SEGS * SEGS];
	for(uint i = 0; i <  256 * SEGS * SEGS; i++)
		hist[i] = 0;

	int seg_height = image.n_rows / SEGS;
    int seg_width  = image.n_cols / SEGS;

	for(uint i = 0; i < SEGS; i++)
        	for(uint j = 0; j < SEGS; j++)
        		for(uint k = i*seg_height + 1; k < (i + 1)*seg_height - 1; k++)
        			for(uint l = j*seg_width + 1; l < (j + 1)*seg_width - 1; l++){
        				if(k >= image.n_rows || l >= image.n_cols)
        					continue;
        				int n = 128*(image(k, l) <= image(k-1, l-1)) + 
        						64 *(image(k, l) <= image(k-1, l))   +
        						32 *(image(k, l) <= image(k-1, l+1)) + 
        						16 *(image(k, l) <= image(k, l+1))   + 
        						8  *(image(k, l) <= image(k+1, l+1)) + 
        						4  *(image(k, l) <= image(k+1, l))   + 
        						2  *(image(k, l) <= image(k+1, l-1)) + 
        						1  *(image(k, l) <= image(k, l-1));
        				hist[i * SEGS * 256 + j * 256 + n] ++;
        			}

    for (int i = 0; i < SEGS*SEGS; i++){
    	double sum = 0;

    	for(uint k = 0; k < 256; k++)
    		sum += hist[i*256 + k] * hist[i*256 + k];

    	sum = sqrt(sum);

    	for(uint k = 0; k < 256; k++){
    		if(sum >= 0.00001f)
    			hist[i*256 + k] /= sum;
    		vec.push_back(hist[i*256 + k]);
    	}
    }
}

void ColourFeatures(BMP* image, vector<float>& vec){
	int height = image->TellHeight();
	int with = image->TellWidth();

	int seg_height = height / 8;
    int seg_width  = with / 8;

    for(uint i = 0; i < 8; i++)
        	for(uint j = 0; j < 8; j++){
        		double mean_r = 0;
        		double mean_g = 0;
        		double mean_b = 0;

        		for(uint k = i*seg_width; k < (i + 1)*seg_width; k++)
        			for(uint l = j*seg_height; l < (j + 1)*seg_height; l++){
        				RGBApixel pix = image->GetPixel(k, l);
        				mean_r += pix.Red;
        				mean_g += pix.Green;
        				mean_b += pix.Blue;
        			}

        		mean_r /= seg_width*seg_height*255;
        		mean_g /= seg_width*seg_height*255;
        		mean_b /= seg_width*seg_height*255;

        		vec.push_back(mean_r);
        		vec.push_back(mean_g);
        		vec.push_back(mean_b);
        	}
}

// Load list of files and its labels from 'data_file' and
// stores it in 'file_list'
void LoadFileList(const string& data_file, TFileList* file_list) {
    ifstream stream(data_file.c_str());

    string filename;
    int label;
    
    int char_idx = data_file.size() - 1;
    for (; char_idx >= 0; --char_idx)
        if (data_file[char_idx] == '/' || data_file[char_idx] == '\\')
            break;
    string data_path = data_file.substr(0,char_idx+1);
    
    while(!stream.eof() && !stream.fail()) {
        stream >> filename >> label;
        if (filename.size())
            file_list->push_back(make_pair(data_path + filename, label));
    }

    stream.close();
}

// Load images by list of files 'file_list' and store them in 'data_set'
void LoadImages(const TFileList& file_list, TDataSet* data_set) {
    for (size_t img_idx = 0; img_idx < file_list.size(); ++img_idx) {
            // Create image
        BMP* image = new BMP();
            // Read image from file
        image->ReadFromFile(file_list[img_idx].first.c_str());
            // Add image and it's label to dataset
        data_set->push_back(make_pair(image, file_list[img_idx].second));
    }
}

// Save result of prediction to file
void SavePredictions(const TFileList& file_list,
                     const TLabels& labels, 
                     const string& prediction_file) {
        // Check that list of files and list of labels has equal size 
    assert(file_list.size() == labels.size());
        // Open 'prediction_file' for writing
    ofstream stream(prediction_file.c_str());

        // Write file names and labels to stream
    for (size_t image_idx = 0; image_idx < file_list.size(); ++image_idx)
        stream << file_list[image_idx].first << " " << labels[image_idx] << endl;
    stream.close();
}

// Exatract features from dataset.
// You should implement this function by yourself =)
void ExtractFeatures(const TDataSet& data_set, TFeatures* features) {
    for (size_t image_idx = 0; image_idx < data_set.size(); ++image_idx) {
        
        BMP* image_bmp = data_set[image_idx].first;

        Matrix<double> grey_image(image_bmp->TellHeight(), image_bmp->TellWidth());

        toGreyScale(image_bmp, grey_image);
	        
        Matrix<double> grad_image(grey_image.n_rows, grey_image.n_cols);
        Matrix<double> dir_image(grey_image.n_rows, grey_image.n_cols);

        getGrad(grey_image, grad_image, dir_image);

        float features_arr[SEGS * SEGS * DIRS];
        for(uint i = 0; i < SEGS * SEGS * DIRS; i++)
        	features_arr[i] = 0;

        int seg_height = grey_image.n_rows / SEGS;
        int seg_width  = grey_image.n_cols / SEGS;

        for(uint i = 0; i < SEGS; i++)
        	for(uint j = 0; j < SEGS; j++)
        		for(uint k = i*seg_height; k < (i + 1)*seg_height; k++)
        			for(uint l = j*seg_width; l < (j + 1)*seg_width; l++){
        				if(k >= grey_image.n_rows || l >= grey_image.n_cols)
        					continue;
        				int n = getDir(dir_image(k, l));
        				features_arr[i*SEGS*DIRS + j*DIRS + n] += grad_image(k, l);
        			}

        for(uint i = 0; i < SEGS*SEGS; i++){
        	double sum = 0;
        	for(uint k = 0; k < DIRS; k++)
        		sum += features_arr[i*DIRS + k] * features_arr[i*DIRS + k];
        	sum = sqrt(sum);
        	if(sum < 0.00001f) continue;
        	for(uint k = 0; k < DIRS; k++)
        		features_arr[i*DIRS + k] /= sum;
        }

        vector<float> vec(features_arr, features_arr + sizeof(features_arr)/sizeof(features_arr[0]));

        LBP(grey_image, vec);

        ColourFeatures(image_bmp, vec);
        
        features->push_back(make_pair(vec, data_set[image_idx].second));
    }
}

// Clear dataset structure
void ClearDataset(TDataSet* data_set) {
        // Delete all images from dataset
    for (size_t image_idx = 0; image_idx < data_set->size(); ++image_idx)
        delete (*data_set)[image_idx].first;
        // Clear dataset
    data_set->clear();
}

// Train SVM classifier using data from 'data_file' and save trained model
// to 'model_file'
void TrainClassifier(const string& data_file, const string& model_file) {
        // List of image file names and its labels
    TFileList file_list;
        // Structure of images and its labels
    TDataSet data_set;
        // Structure of features of images and its labels
    TFeatures features;
        // Model which would be trained
    TModel model;
        // Parameters of classifier
    TClassifierParams params;
    
        // Load list of image file names and its labels
    LoadFileList(data_file, &file_list);
        // Load images
    LoadImages(file_list, &data_set);
        // Extract features from images
    ExtractFeatures(data_set, &features);

        // PLACE YOUR CODE HERE
        // You can change parameters of classifier here
    params.C = 0.01;
    TClassifier classifier(params);
        // Train classifier
    classifier.Train(features, &model);
        // Save model to file
    model.Save(model_file);
        // Clear dataset structure
    ClearDataset(&data_set);
}

// Predict data from 'data_file' using model from 'model_file' and
// save predictions to 'prediction_file'
void PredictData(const string& data_file,
                 const string& model_file,
                 const string& prediction_file) {
        // List of image file names and its labels
    TFileList file_list;
        // Structure of images and its labels
    TDataSet data_set;
        // Structure of features of images and its labels
    TFeatures features;
        // List of image labels
    TLabels labels;

        // Load list of image file names and its labels
    LoadFileList(data_file, &file_list);
        // Load images
    LoadImages(file_list, &data_set);
        // Extract features from images
    ExtractFeatures(data_set, &features);

        // Classifier 
    TClassifier classifier = TClassifier(TClassifierParams());
        // Trained model
    TModel model;
        // Load model from file
    model.Load(model_file);
        // Predict images by its features using 'model' and store predictions
        // to 'labels'
    classifier.Predict(features, model, &labels);

        // Save predictions
    SavePredictions(file_list, labels, prediction_file);
        // Clear dataset structure
    ClearDataset(&data_set);
}

int main(int argc, char** argv) {
    // Command line options parser
    ArgvParser cmd;
        // Description of program
    cmd.setIntroductoryDescription("Machine graphics course, task 2. CMC MSU, 2014.");
        // Add help option
    cmd.setHelpOption("h", "help", "Print this help message");
        // Add other options
    cmd.defineOption("data_set", "File with dataset",
        ArgvParser::OptionRequiresValue | ArgvParser::OptionRequired);
    cmd.defineOption("model", "Path to file to save or load model",
        ArgvParser::OptionRequiresValue | ArgvParser::OptionRequired);
    cmd.defineOption("predicted_labels", "Path to file to save prediction results",
        ArgvParser::OptionRequiresValue);
    cmd.defineOption("train", "Train classifier");
    cmd.defineOption("predict", "Predict dataset");
        
        // Add options aliases
    cmd.defineOptionAlternative("data_set", "d");
    cmd.defineOptionAlternative("model", "m");
    cmd.defineOptionAlternative("predicted_labels", "l");
    cmd.defineOptionAlternative("train", "t");
    cmd.defineOptionAlternative("predict", "p");

        // Parse options
    int result = cmd.parse(argc, argv);

        // Check for errors or help option
    if (result) {
        cout << cmd.parseErrorDescription(result) << endl;
        return result;
    }

        // Get values 
    string data_file = cmd.optionValue("data_set");
    string model_file = cmd.optionValue("model");
    bool train = cmd.foundOption("train");
    bool predict = cmd.foundOption("predict");

        // If we need to train classifier
    if (train)
        TrainClassifier(data_file, model_file);
        // If we need to predict data
    if (predict) {
            // You must declare file to save images
        if (!cmd.foundOption("predicted_labels")) {
            cerr << "Error! Option --predicted_labels not found!" << endl;
            return 1;
        }
            // File to save predictions
        string prediction_file = cmd.optionValue("predicted_labels");
            // Predict data
        PredictData(data_file, model_file, prediction_file);
    }
}
