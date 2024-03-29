#include <wx/wx.h>
#include "window.h"
#include <wx/image.h>
#include <wx/dcmemory.h>
#include <wx/dcclient.h>
#include <wx/dc.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include <fstream>
#include <string>
#include <wx/string.h>
#include <stdio.h>
#include <math.h> 
#include <wx/textdlg.h>
#include <wx/msgdlg.h> 
#include <map>

using namespace std; 

static const wxChar *FILETYPES = _T("All files (*.*)|*.*");

IMPLEMENT_APP(BasicApplication)

bool BasicApplication::OnInit()
{
  wxInitAllImageHandlers();
  MyFrame *frame = new MyFrame(_("Basic Frame"), 50, 50, 700, 400);
  frame->Show(TRUE);
  SetTopWindow(frame);
  return TRUE;	
}

MyFrame::MyFrame(const wxString title, int xpos, int ypos, int width, int height): wxFrame((wxFrame *) NULL, -1, title, wxPoint(xpos, ypos), wxSize(width, height)){

  fileMenu = new wxMenu;
  editMenu = new wxMenu;
  scaleMenu = new wxMenu;
  filterMenu = new wxMenu;
  pointMenu = new wxMenu;
  histogramMenu = new wxMenu;
  selection = new wxRect;
  
  fileMenu->Append(LOAD_FILE_ID, _T("&Open file"));
  fileMenu->Append(LOAD_RAW, _T("&Open Raw File"));  
//###########################################################//
//----------------------START MY MENU -----------------------//
//###########################################################// 
  
  editMenu->Append(UNDO_ID, _T("&Undo"));
  
  scaleMenu->Append(INVERT_IMAGE_ID, _T("&Invert image"));
  scaleMenu->Append(SCALE_IMAGE_ID, _T("&Scale image"));
  scaleMenu->Append(SHIFT_IMAGE_ID, _T("&Shift image"));
  
  filterMenu->Append(CONVOLUTION_ID, _T("&Convolution"));
  filterMenu->Append(SALT_PEPPER_ID, _T("&Add Salt and Pepper"));
  filterMenu->Append(MIN_FILTER_ID, _T("&Min Filter"));
  filterMenu->Append(MAX_FILTER_ID, _T("&Max Filter"));
  filterMenu->Append(MID_POINT_ID, _T("&Midpoint Filter"));
  
  pointMenu->Append(LOGARITHM_ID, _T("&Logarithm Function"));
  pointMenu->Append(POWER_ID, _T("&Power Function"));
  pointMenu->Append(NEGATIVE_ID, _T("&Negative Linear Transform"));
  
  histogramMenu->Append(LUT_ID, _T("&Random Lookup Table"));
  histogramMenu->Append(HIST_EQ_ID, _T("&Histogram Equalisation"));
  histogramMenu->Append(HIST_STAT_ID, _T("&Histogram Statistics"));
  histogramMenu->Append(SIMPLE_THRESH_ID, _T("&Simple Thresholding"));
  histogramMenu->Append(AUTO_THRESH_ID, _T("&Automated Thresholding"));
  
//###########################################################//
//----------------------END MY MENU -------------------------//
//###########################################################// 

  fileMenu->AppendSeparator();
  fileMenu->Append(SAVE_IMAGE_ID, _T("&Save image"));
  fileMenu->Append(EXIT_ID, _T("&Exit"));

  menuBar = new wxMenuBar;
  menuBar->Append(fileMenu, _T("&File"));
  menuBar->Append(editMenu, _T("&Edit"));
  menuBar->Append(scaleMenu, _T("&Scaling and Shifting"));
  menuBar->Append(filterMenu, _T("&Filters and Masks"));
  menuBar->Append(pointMenu, _T("&Point Functions"));
  menuBar->Append(histogramMenu, _T("&Histograms"));
  
  SetMenuBar(menuBar);
  CreateStatusBar(3); 
  oldWidth = 0;
  oldHeight = 0;
  loadedImage = 0;
  undoImage = 0;

/* initialise the variables that we added */
  imgWidth = imgHeight = 0;
  stuffToDraw = 0;

}

MyFrame::~MyFrame(){
/* release resources */
  if(loadedImage){
    loadedImage->Destroy();
    loadedImage = 0;
    undoImage->Destroy();
    undoImage = 0;
  }

}

void MyFrame::OnOpenFile(wxCommandEvent & event){
  wxFileDialog *openFileDialog = new wxFileDialog ( this, _T("Open file"), _T(""), _T(""), FILETYPES, wxOPEN, wxDefaultPosition);  
  if(openFileDialog->ShowModal() == wxID_OK){
    wxString filename = openFileDialog->GetFilename();
    wxString path = openFileDialog->GetPath();
    printf("Loading image from file...");    
    loadedImage = new wxImage(path); //Image Loaded form file 
    undoImage = new wxImage(path);
    if(loadedImage->Ok()){
      stuffToDraw = ORIGINAL_IMG;    // set the display flag
      printf("Done! \n");    
    }
    else {
      printf("error:...");
      loadedImage->Destroy();
      undoImage->Destroy();
      loadedImage = 0;
      undoImage = 0;
    }
    Refresh();
  }    
}


//###########################################################//
//-----------------------------------------------------------//
//---------- DO NOT MODIFY THE CODE ABOVE--------------------//
//-----------------------------------------------------------//
//###########################################################//

//OPEN RAW FILE
void MyFrame::OpenRawFile(wxCommandEvent & event){

    wxFileDialog *openFileDialog = new wxFileDialog ( this, _T("Open file"), _T(""), _T(""), FILETYPES, wxOPEN, wxDefaultPosition);  
    if(openFileDialog->ShowModal() == wxID_OK)
    {
        wxString filename = openFileDialog->GetFilename();
        wxString path = openFileDialog->GetPath();

        FILE * pFile;
        pFile = fopen (path.char_str(), "r");
        int size;

        //obtain file size
        fseek(pFile, 0, SEEK_END);
        size = ftell(pFile);
        fseek(pFile, 0, SEEK_SET);

        //instantiate buffer with size
        unsigned char * buffer = new unsigned char[size];
        unsigned char * nbuff = new unsigned char[size*3];

        //copy file into the buffer
        fread(buffer, sizeof(char), size, pFile);               
        fclose(pFile);
        
        int j = 0;
        for(int i=0; i < size*3; i = i + 3)
        {
            nbuff[i] = buffer[j];
            nbuff[i+1] = buffer[j];
            nbuff[i+2] = buffer[j];
            j++;
        }
        

        //assume square images, dimension is the square root of the file size
        int dimension = sqrt(size);
        free (buffer);

        loadedImage = new wxImage(dimension, dimension, nbuff, false);
        undoImage = new wxImage(dimension, dimension, nbuff, false);
        if(loadedImage->Ok())
        {
          stuffToDraw = ORIGINAL_IMG;    // set the display flag
          printf("Done! \n");    
        }
        else 
        {
          printf("error:...");
          loadedImage->Destroy();
          loadedImage = 0;
          undoImage->Destroy();
          undoImage = 0;
        }
        
        Refresh();
    }
}


//INVERT IMAGE
void MyFrame::OnInvertImage(wxCommandEvent & event){
  
    free(loadedImage);
    loadedImage = new wxImage(bitmap.ConvertToImage());
    undoImage = new wxImage(bitmap.ConvertToImage());
    
    int x = 0;
    int y = 0;
    int width = imgWidth;
    int height = imgHeight;
    
    if(!selection->IsEmpty()){
        x = selection->GetTopLeft().x;
        y = selection->GetTopLeft().y;
        width = selection->GetBottomRight().x;
        height = selection->GetBottomRight().y;
    }

    for(int i = x;i<width;i++){
       for(int j = y;j<height;j++){
        loadedImage->SetRGB(i,j,255-loadedImage->GetRed(i,j), 
                                255-loadedImage->GetGreen(i,j),
                                255-loadedImage->GetBlue(i,j));
       }
    }

    printf(" Finished inverting.\n");
    Refresh();
}

//IMAGE SCALING
void MyFrame::OnScaleImage(wxCommandEvent & event){

    wxString scaleInput = wxGetTextFromUser (
            wxT("Enter a scaling factor"),
            wxT("Prompt"),
            wxT(""),
            NULL,
            -1, -1, TRUE                
            ); 
    double scale;
    scaleInput.ToDouble(&scale);    
    
    free(loadedImage);
    loadedImage = new wxImage(bitmap.ConvertToImage());
    undoImage = new wxImage(bitmap.ConvertToImage());
    
    //initialise min-max for each colour
    double max[3];
    double min[3];
    for(int i=0; i<3;i++)
    {
        max[i] = 0.0;
        min[i] = 255.0;
    }
    
    int x = 0;
    int y = 0;
    int width = imgWidth;
    int height = imgHeight;
    
    if(!selection->IsEmpty()){
        x = selection->GetTopLeft().x;
        y = selection->GetTopLeft().y;
        width = selection->GetBottomRight().x;
        height = selection->GetBottomRight().y;
    }

    //get min-max value for each colour
    for(int i = x;i<width;i++){
            for(int j = y;j<height;j++){

                double r = (double)(scale * loadedImage->GetRed(i,j));
                double g = (double)(scale * loadedImage->GetGreen(i,j));
                double b = (double)(scale * loadedImage->GetBlue(i,j));
                
                if(r > max[0]) max[0] = r;
                if(r < min[0]) min[0] = r;
                
                if(g > max[1]) max[1] = g;
                if(g < min[1]) min[1] = g;
                
                if(b > max[2]) max[2] = b;
                if(b < min[2]) min[2] = b;
            }
        }
        
        //rescale pixels and output.
        for(int i = x;i<width;i++){
                for(int j = y;j<height;j++){

                double r = (double)(scale * loadedImage->GetRed(i,j));
                double g = (double)(scale * loadedImage->GetGreen(i,j));
                double b = (double)(scale * loadedImage->GetBlue(i,j));
                              
                double outputRed = Rescale(r, min[0], max[0]);
                double outputGreen = Rescale(g, min[1], max[1]);
                double outputBlue = Rescale(b, min[2], max[2]);
                
                //Set output value
                loadedImage->SetRGB(i,j,outputRed,outputGreen,outputBlue);
            }
        }
	
    printf(" Finished scaling.\n");
    Refresh();
}

//IMAGE SHIFTING
void MyFrame::OnShiftImage(wxCommandEvent & event){

    wxString scaleInput = wxGetTextFromUser (
            wxT("Enter a shifting factor from -255 to 255"),
            wxT("Prompt"),
            wxT(""),
            NULL,
            -1, -1, TRUE                
            ); 
    double shift;
    scaleInput.ToDouble(&shift);

    if(shift >= -255 && shift <= 255)
    {
        cout << "Shifting by " << string(scaleInput.mb_str()) << "...\n" << endl; 
        free(loadedImage);
        loadedImage = new wxImage(bitmap.ConvertToImage());
        undoImage = new wxImage(bitmap.ConvertToImage());
        
        int x = 0;
        int y = 0;
        int width = imgWidth;
        int height = imgHeight;

        if(!selection->IsEmpty()){
            x = selection->GetTopLeft().x;
            y = selection->GetTopLeft().y;
            width = selection->GetBottomRight().x;
            height = selection->GetBottomRight().y;
        }

        for(int i = x;i<width;i++){
            for(int j = y;j<height;j++){
               
            loadedImage->SetRGB(i,j,max(min(shift + loadedImage->GetRed(i,j), 255.0), 0.0), 
                                    max(min(shift + loadedImage->GetGreen(i,j), 255.0), 0.0),
                                    max(min(shift + loadedImage->GetBlue(i,j), 255.0), 0.0));
           }
        }

        printf(" Finished shifting.\n");
        Refresh();
    }
    else
    {
        wxMessageBox( wxT("Invalid Input."), wxT("oops!"), wxICON_EXCLAMATION);
    }
}

//ConvolutionMenu
void MyFrame::OnConvolution(wxCommandEvent & event){
    wxString scaleInput = wxGetTextFromUser (
            wxT("Enter number corresponding to mask:"
            "\n 1. Averaging"
            "\n 2. Weighted Averaging"
            "\n 3. 4-neighbour Laplacian"
            "\n 4. 8-neighbour Laplacian"
            "\n 5. 4-neighbour Laplacian Enhancement"
            "\n 6. 8-neighbour Laplacian Enhancement"
            "\n 7. Roberts X"
            "\n 8. Roberts Y"
            "\n 9. Sobel X"
            "\n 10. Sobel Y"),
            wxT("Prompt"),
            wxT(""),
            NULL,
            -1, -1, TRUE                
            ); 
    double input;
    scaleInput.ToDouble(&input);
    int selection = static_cast<int>(input + 0.5);
    
    if(selection > 10 || selection < 1)
    {
        wxMessageBox( wxT("Invalid Input."), wxT("oops!"), wxICON_EXCLAMATION);
    }
    else
    {
        double mask [3][3];
        switch(selection)
        {
            //Averaging
            case 1 :
                for(int i = 0; i < 3; i++)
                {
                    for(int j = 0; j < 3; j++)
                    {
                        mask[i][j] = 1.0/9.0;
                    }
                }
                MyFrame::Convolution(mask, false);
                break;
            //Weighted Averaging
            case 2 :
                mask[0][0] = 1.0/16.0; mask[0][1] = 2.0/16.0; mask[0][2] = 1.0/16.0;
                mask[1][0] = 2.0/16.0; mask[1][1] = 4.0/16.0; mask[1][2] = 2.0/16.0;
                mask[2][0] = 1.0/16.0; mask[2][1] = 2.0/16.0; mask[2][2] = 1.0/16.0;
                MyFrame::Convolution(mask, false);
                break;
            //4N Laplacian
            case 3 :
                mask[0][0] = 0.0; mask[0][1] = -1.0; mask[0][2] = 0.0;
                mask[1][0] = -1.0; mask[1][1] = 4.0; mask[1][2] = -1.0;
                mask[2][0] = 0.0; mask[2][1] = -1.0; mask[2][2] = 0.0;
                MyFrame::Convolution(mask, false);
                break;
            //8N Laplacian
            case 4 :
                mask[0][0] = -1.0; mask[0][1] = -1.0; mask[0][2] = -1.0;
                mask[1][0] = -1.0; mask[1][1] = 8.0; mask[1][2] = -1.0;
                mask[2][0] = -1.0; mask[2][1] = -1.0; mask[2][2] = -1.0;
                MyFrame::Convolution(mask, false);
                break;
            //4N Laplacian Enhancement
            case 5 :
                mask[0][0] = 0.0; mask[0][1] = -1.0; mask[0][2] = 0.0;
                mask[1][0] = -1.0; mask[1][1] = 5.0; mask[1][2] = -1.0;
                mask[2][0] = 0.0; mask[2][1] = -1.0; mask[2][2] = 0.0;
                MyFrame::Convolution(mask, false);
               break;
            //8N Laplacian Enhancement
            case 6 :
                mask[0][0] = -1.0; mask[0][1] = -1.0; mask[0][2] = -1.0;
                mask[1][0] = -1.0; mask[1][1] = 9.0; mask[1][2] = -1.0;
                mask[2][0] = -1.0; mask[2][1] = -1.0; mask[2][2] = -1.0;
                MyFrame::Convolution(mask, false);
                break;
            //Roberts
            case 7 :
                mask[0][0] = 0.0; mask[0][1] = 0.0; mask[0][2] = 0.0;
                mask[1][0] = 0.0; mask[1][1] = 0.0; mask[1][2] = -1.0;
                mask[2][0] = 0.0; mask[2][1] = 1.0; mask[2][2] = 0.0;
                MyFrame::Convolution(mask, true);
                break;
            //Roberts
            case 8:
                mask[0][0] = 0.0; mask[0][1] = 0.0; mask[0][2] = 0.0;
                mask[1][0] = 0.0; mask[1][1] = -1.0; mask[1][2] = 0.0;
                mask[2][0] = 0.0; mask[2][1] = 0.0; mask[2][2] = 1.0;
                MyFrame::Convolution(mask, true);
                break;
            //Sobel X
            case 9:
                mask[0][0] = -1.0; mask[0][1] = 0.0; mask[0][2] = 1.0;
                mask[1][0] = -2.0; mask[1][1] = 0.0; mask[1][2] = 2.0;
                mask[2][0] = -1.0; mask[2][1] = 0.0; mask[2][2] = 1.0;
                MyFrame::Convolution(mask, true);
                break;
            //Sobel Y
            case 10:
                mask[0][0] = -1.0; mask[0][1] = -2.0; mask[0][2] = -1.0;
                mask[1][0] = 0.0; mask[1][1] = 0.0; mask[1][2] = 0.0;
                mask[2][0] = 1.0; mask[2][1] = 2.0; mask[2][2] = 1.0;
                MyFrame::Convolution(mask, true);
                break;
            default :
                wxMessageBox( wxT("Invalid Input."), wxT("oops!"), wxICON_EXCLAMATION);
        }
        cout << "Finished convolution";
        Refresh();
    }
    
}

//Convolution Function
void MyFrame::Convolution(double mask[3][3], bool absValConversion)
{
    cout << endl << "Mask:" << endl;
    for (int m=0; m<3; m++)
    {
        for(int n=0; n<3; n++)
        {
                cout << mask[m][n] << "\t";
        }
        cout << endl;
    }
    
    free(loadedImage);
    loadedImage = new wxImage(bitmap.ConvertToImage());
    undoImage = new wxImage(bitmap.ConvertToImage());
    wxImage *tempImage = new wxImage(bitmap.ConvertToImage());
    
    int x = 0;
    int y = 0;
    int width = imgWidth;
    int height = imgHeight;

    if(!selection->IsEmpty()){
        x = selection->GetTopLeft().x;
        y = selection->GetTopLeft().y;
        width = selection->GetBottomRight().x;
        height = selection->GetBottomRight().y;
    }
    
    for(int i = x;i<width;i++){
        for(int j = y;j<height;j++){                 
            //Computes weighted average using mask values as weights
            double averageRed = 0.0;
            double averageBlue = 0.0;
            double averageGreen = 0.0;
            
            //RED
            //row 0
            averageRed += mask[0][0] * tempImage->GetRed(i-1,j-1);
            averageRed += mask[0][1] * tempImage->GetRed(i,j-1);
            averageRed += mask[0][2] * tempImage->GetRed(i+1,j-1);
            //row 1
            averageRed += mask[1][0] * tempImage->GetRed(i-1,j);
            averageRed += mask[1][1] * tempImage->GetRed(i,j);
            averageRed += mask[1][2] * tempImage->GetRed(i+1,j);
            //row 2
            averageRed += mask[2][0] * tempImage->GetRed(i-1,j+1);
            averageRed += mask[2][1] * tempImage->GetRed(i,j+1);
            averageRed += mask[2][2] * tempImage->GetRed(i+1,j+1);
            
            //BLUE
            //row 0
            averageBlue += mask[0][0] * tempImage->GetBlue(i-1,j-1);
            averageBlue += mask[0][1] * tempImage->GetBlue(i,j-1);
            averageBlue += mask[0][2] * tempImage->GetBlue(i+1,j-1);
            //row 1
            averageBlue += mask[1][0] * tempImage->GetBlue(i-1,j);
            averageBlue += mask[1][1] * tempImage->GetBlue(i,j);
            averageBlue += mask[1][2] * tempImage->GetBlue(i+1,j);
            //row 2
            averageBlue += mask[2][0] * tempImage->GetBlue(i-1,j+1);
            averageBlue += mask[2][1] * tempImage->GetBlue(i,j+1);
            averageBlue += mask[2][2] * tempImage->GetBlue(i+1,j+1);
            
            //GREEN
            //row 0
            averageGreen += mask[0][0] * tempImage->GetGreen(i-1,j-1);
            averageGreen += mask[0][1] * tempImage->GetGreen(i,j-1);
            averageGreen += mask[0][2] * tempImage->GetGreen(i+1,j-1);
            //row 1
            averageGreen += mask[1][0] * tempImage->GetGreen(i-1,j);
            averageGreen += mask[1][1] * tempImage->GetGreen(i,j);
            averageGreen += mask[1][2] * tempImage->GetGreen(i+1,j);
            //row 2
            averageGreen += mask[2][0] * tempImage->GetGreen(i-1,j+1);
            averageGreen += mask[2][1] * tempImage->GetGreen(i,j+1);
            averageGreen += mask[2][2] * tempImage->GetGreen(i+1,j+1);
            
            
            if(absValConversion)
            {
                averageRed = abs(averageRed);
                averageGreen = abs(averageGreen);
                averageBlue = abs(averageBlue);
            }
            else
            {
                averageRed = max(min(averageRed, 255.0), 0.0);
                averageGreen = max(min(averageGreen, 255.0), 0.0);
                averageBlue = max(min(averageBlue, 255.0), 0.0);
            }
            
            //Set output value
            loadedImage->SetRGB(i,j,averageRed,averageGreen,averageBlue);
        }
    }
}

//Add Salt & PEPPER
void MyFrame::AddSaltPepper(wxCommandEvent & event){
    
    free(loadedImage);
    loadedImage = new wxImage(bitmap.ConvertToImage());
    undoImage = new wxImage(bitmap.ConvertToImage());
    int intensity = 0;
    
    wxString scaleInput = wxGetTextFromUser (
            wxT("Enter number of salt and pepper particles to add:"),
            wxT("Prompt"),
            wxT(""),
            NULL,
            -1, -1, TRUE                
            ); 
    double input;
    scaleInput.ToDouble(&input);
    intensity = static_cast<int>(input + 0.5);
    
    if(intensity >= 1) {
        for(int i=0; i < intensity; i++)
        {
            //select random coordinates
            int randomX = rand() % imgWidth;
            int randomY = rand() % imgHeight;
            //select if salt or pepper
            int saltOrPepper = rand() % 100;
            
            if(saltOrPepper > 50)
            {
               loadedImage->SetRGB(randomX,randomY,255,255,255); 
            }
            else
            {
                loadedImage->SetRGB(randomX,randomY,0,0,0);
            }
        }
        printf(" Finished Adding Salt & Pepper.\n");
        Refresh();
        
    } else {
        wxMessageBox( wxT("Invalid Input."), wxT("oops!"), wxICON_EXCLAMATION);
    }  
}

void MyFrame::MinFilter(wxCommandEvent & event){
    
    int neighbours = 1;
    
    wxString scaleInput = wxGetTextFromUser (
            wxT("Enter number of neighbours\ne.g. 1 = 3x3 area"),
            wxT("Prompt"),
            wxT(""),
            NULL,
            -1, -1, TRUE                
            ); 
    double input;
    scaleInput.ToDouble(&input);
    neighbours = static_cast<int>(input + 0.5);
    
    if(neighbours >= 1){
    
        free(loadedImage);
        loadedImage = new wxImage(bitmap.ConvertToImage());
        undoImage = new wxImage(bitmap.ConvertToImage());
        wxImage *tempImage = new wxImage(bitmap.ConvertToImage());

        int x = 0;
        int y = 0;
        int width = imgWidth;
        int height = imgHeight;

        if(!selection->IsEmpty()){
            x = selection->GetTopLeft().x;
            y = selection->GetTopLeft().y;
            width = selection->GetBottomRight().x;
            height = selection->GetBottomRight().y;
        }

        for(int i = x;i<width;i++){
            for(int j = y;j<height;j++){ 

                int minRed = 255;
                int minBlue = 255;
                int minGreen = 255;                

                //RED
                for(int k= -1*neighbours; k<=neighbours; k++){
                    for(int l = -1*neighbours; l<=neighbours; l++){
                        minRed = min(minRed, (int)tempImage->GetRed(i+l,j+k));
                    }
                }
                
                //BLUE
                for(int k= -1*neighbours; k<=neighbours; k++){
                    for(int l = -1*neighbours; l<=neighbours; l++){
                        minBlue = min(minBlue, (int)tempImage->GetBlue(i+l,j+k));
                    }
                }
                
                //GREEN
                for(int k= -1*neighbours; k<=neighbours; k++){
                    for(int l = -1*neighbours; l<=neighbours; l++){
                        minGreen = min(minGreen, (int)tempImage->GetGreen(i+l,j+k));
                    }
                }
              
                //Set output value
                loadedImage->SetRGB(i,j,minRed,minGreen,minBlue);
            }
        }
        
        printf(" Finished Min Filter function.\n");
        Refresh();
        
    } else {
        wxMessageBox( wxT("Invalid Input."), wxT("oops!"), wxICON_EXCLAMATION);
    }
}

void MyFrame::MaxFilter(wxCommandEvent & event){
    
    int neighbours = 1;
    
    wxString scaleInput = wxGetTextFromUser (
            wxT("Enter number of neighbours\ne.g. 1 = 3x3 area"),
            wxT("Prompt"),
            wxT(""),
            NULL,
            -1, -1, TRUE                
            ); 
    double input;
    scaleInput.ToDouble(&input);
    neighbours = static_cast<int>(input + 0.5);
    
    if(neighbours >= 1){
    
        free(loadedImage);
        loadedImage = new wxImage(bitmap.ConvertToImage());
        undoImage = new wxImage(bitmap.ConvertToImage());
        wxImage *tempImage = new wxImage(bitmap.ConvertToImage());

        int x = 0;
        int y = 0;
        int width = imgWidth;
        int height = imgHeight;

        if(!selection->IsEmpty()){
            x = selection->GetTopLeft().x;
            y = selection->GetTopLeft().y;
            width = selection->GetBottomRight().x;
            height = selection->GetBottomRight().y;
        }

        for(int i = x;i<width;i++){
            for(int j = y;j<height;j++){ 

                int maxRed = 0;
                int maxBlue = 0;
                int maxGreen = 0;

                //RED
                    for(int k= -1*neighbours; k<=neighbours; k++){
                        for(int l = -1*neighbours; l<=neighbours; l++){
                            maxRed = max(maxRed, (int)tempImage->GetRed(i+l,j+k));
                        }
                    }

                    //BLUE
                    for(int k= -1*neighbours; k<=neighbours; k++){
                        for(int l = -1*neighbours; l<=neighbours; l++){
                            maxBlue = max(maxBlue, (int)tempImage->GetBlue(i+l,j+k));
                        }
                    }

                    //GREEN
                    for(int k= -1*neighbours; k<=neighbours; k++){
                        for(int l = -1*neighbours; l<=neighbours; l++){
                            maxGreen = max(maxGreen, (int)tempImage->GetGreen(i+l,j+k));
                        }
                    }

                //Set output value
                loadedImage->SetRGB(i,j,maxRed,maxGreen,maxBlue);
            }
        }
        
    printf(" Finished Max Filter function.\n");
    Refresh();
        
    }else {
        wxMessageBox( wxT("Invalid Input."), wxT("oops!"), wxICON_EXCLAMATION);
    }
}

void MyFrame::MidPointFilter(wxCommandEvent & event){
    
    int neighbours = 0;
    
    wxString scaleInput = wxGetTextFromUser (
            wxT("Enter number of neighbours\ne.g. 1 = 3x3 area"),
            wxT("Prompt"),
            wxT(""),
            NULL,
            -1, -1, TRUE                
            ); 
    double input;
    scaleInput.ToDouble(&input);
    neighbours = static_cast<int>(input + 0.5);
    
    if(neighbours >= 1){

        free(loadedImage);
        loadedImage = new wxImage(bitmap.ConvertToImage());
        undoImage = new wxImage(bitmap.ConvertToImage());
        wxImage *tempImage = new wxImage(bitmap.ConvertToImage());

        int x = 0;
        int y = 0;
        int width = imgWidth;
        int height = imgHeight;

        if(!selection->IsEmpty()){
            x = selection->GetTopLeft().x;
            y = selection->GetTopLeft().y;
            width = selection->GetBottomRight().x;
            height = selection->GetBottomRight().y;
        }

        for(int i = x;i<width;i++){
            for(int j = y;j<height;j++){ 

                int maxRed = 0;
                int maxBlue = 0;
                int maxGreen = 0;
                int minRed = 255;
                int minBlue = 255;
                int minGreen = 255;

                for(int k= -1*neighbours; k<=neighbours; k++){
                    for(int l = -1*neighbours; l<=neighbours; l++){
                        maxRed = max(maxRed, (int)tempImage->GetRed(i+l,j+k));
                        minRed = min(minRed, (int)tempImage->GetRed(i+l,j+k));
                    }
                }

                //BLUE
                for(int k= -1*neighbours; k<=neighbours; k++){
                    for(int l = -1*neighbours; l<=neighbours; l++){
                        maxBlue = max(maxBlue, (int)tempImage->GetBlue(i+l,j+k));
                        minBlue = min(minBlue, (int)tempImage->GetBlue(i+l,j+k));
                    }
                }

                //GREEN
                for(int k= -1*neighbours; k<=neighbours; k++){
                    for(int l = -1*neighbours; l<=neighbours; l++){
                        maxGreen = max(maxGreen, (int)tempImage->GetGreen(i+l,j+k));
                        minGreen = min(minGreen, (int)tempImage->GetGreen(i+l,j+k));
                    }
                }

                int midPointRed = (maxRed + minRed)/2;
                int midPointBlue = (maxBlue + minBlue)/2;
                int midPointGreen = (maxGreen + minGreen)/2;


                //Set output value
                loadedImage->SetRGB(i,j,midPointRed,midPointGreen,midPointBlue);
            }
        }
        
        printf(" Finished Mid-point filter.\n");
        Refresh();
    } 
    else {
        wxMessageBox( wxT("Invalid Input."), wxT("oops!"), wxICON_EXCLAMATION);
    }
    
    
}

void MyFrame::NegativeLinearTransform(wxCommandEvent & event){
    loadedImage = new wxImage(bitmap.ConvertToImage());
    undoImage = new wxImage(bitmap.ConvertToImage());
    
    int x = 0;
    int y = 0;
    int width = imgWidth;
    int height = imgHeight;

    if(!selection->IsEmpty()){
        x = selection->GetTopLeft().x;
        y = selection->GetTopLeft().y;
        width = selection->GetBottomRight().x;
        height = selection->GetBottomRight().y;
    }
    
    for(int i = x;i<width;i++){
        for(int j = y;j<height;j++){ 
 	loadedImage->SetRGB(i,j,255-loadedImage->GetRed(i,j), 
				255-loadedImage->GetGreen(i,j),
				255-loadedImage->GetBlue(i,j));
       }
    }
    printf(" Finished performing negative linear transformation.\n");
    Refresh();
    
}

//compresses values of dark pixels and expands values of bright pixels
void MyFrame::LogTransformation(wxCommandEvent & event){
    
    double constant = 255/log(256);
    
    wxString scaleInput = wxGetTextFromUser (
            wxT("Enter value for logarithm base, 0 for natural log"),
            wxT("Prompt"),
            wxT(""),
            NULL,
            -1, -1, TRUE                
            ); 
    double input;
    scaleInput.ToDouble(&input);
    int base = static_cast<int>(input + 0.5);
    
    if(base > 1 || base == 0){
        
        if(base == 0) base = exp(1);
    
        free(loadedImage);
        loadedImage = new wxImage(bitmap.ConvertToImage());
        undoImage = new wxImage(bitmap.ConvertToImage());
        wxImage *tempImage = new wxImage(bitmap.ConvertToImage());
        
        //initialise min-max for each colour
        double max[3];
        double min[3];
        for(int i=0; i<3;i++)
        {
            max[i] = 0.0;
            min[i] = 255.0;
        }
        
        
        int x = 0;
        int y = 0;
        int width = imgWidth;
        int height = imgHeight;

        if(!selection->IsEmpty()){
            x = selection->GetTopLeft().x;
            y = selection->GetTopLeft().y;
            width = selection->GetBottomRight().x;
            height = selection->GetBottomRight().y;
        }
        
        //get min-max value for each colour
        for(int i = x;i<width;i++){
            for(int j = y;j<height;j++){ 

                double r = (double)(constant * ((log(loadedImage->GetRed(i,j) + 1)) / log(base)));
                double g = (double)(constant * ((log(loadedImage->GetGreen(i,j) + 1)) / log(base)));
                double b = (double)(constant * ((log(loadedImage->GetBlue(i,j) + 1)) / log(base)));
                
                if(r > max[0]) max[0] = r;
                if(r < min[0]) min[0] = r;
                
                if(g > max[1]) max[1] = g;
                if(g < min[1]) min[1] = g;
                
                if(b > max[2]) max[2] = b;
                if(b < min[2]) min[2] = b;
            }
        }
        
        //rescale pixels and output.
        for(int i = x;i<width;i++){
            for(int j = y;j<height;j++){

                double r = (double)(constant * ((log(loadedImage->GetRed(i,j) + 1)) / log(base)));
                double g = (double)(constant * ((log(loadedImage->GetGreen(i,j) + 1)) / log(base)));
                double b = (double)(constant * ((log(loadedImage->GetBlue(i,j) + 1)) / log(base)));
                              
                double outputRed = Rescale(r, min[0], max[0]);
                double outputGreen = Rescale(g, min[1], max[1]);
                double outputBlue = Rescale(b, min[2], max[2]);
                
                //Set output value
                loadedImage->SetRGB(i,j,outputRed,outputGreen,outputBlue);
            }
        }

        printf(" Finished Logarithm transformation.\n");
        Refresh();
    }
    else{
        wxMessageBox( wxT("Invalid Input."), wxT("oops!"), wxICON_EXCLAMATION);
    }

}

//expand values of dark pixels and compress values of bright pixels (for p > 1)
void MyFrame::PowerTransformation(wxCommandEvent & event){
    
    double constant = 8;
    double power = -1;
    
    wxString scaleInput = wxGetTextFromUser (
            wxT("Enter value for power"),
            wxT("Prompt"),
            wxT(""),
            NULL,
            -1, -1, TRUE                
            ); 
    scaleInput.ToDouble(&power);
    
    if(power > 0){
    
        free(loadedImage);
        loadedImage = new wxImage(bitmap.ConvertToImage());
        undoImage = new wxImage(bitmap.ConvertToImage());

        //initialise min-max for each colour
        double max[3];
        double min[3];
        for(int i=0; i<3;i++)
        {
            max[i] = 0.0;
            min[i] = pow(255, power);
        }
        
        int x = 0;
        int y = 0;
        int width = imgWidth;
        int height = imgHeight;

        if(!selection->IsEmpty()){
            x = selection->GetTopLeft().x;
            y = selection->GetTopLeft().y;
            width = selection->GetBottomRight().x;
            height = selection->GetBottomRight().y;
        }
        
        //get min-max value for each colour
        for(int i = x;i<width;i++){
            for(int j = y;j<height;j++){

                double r = pow((double)loadedImage->GetRed(i,j), power);
                double g = pow((double)loadedImage->GetGreen(i,j), power);
                double b = pow((double)loadedImage->GetBlue(i,j), power);
                
                if(r > max[0]) max[0] = r;
                if(r < min[0]) min[0] = r;
                
                if(g > max[1]) max[1] = g;
                if(g < min[1]) min[1] = g;
                
                if(b > max[2]) max[2] = b;
                if(b < min[2]) min[2] = b;
            }
        }
        
        //rescale pixels and output.
        for(int i = x;i<width;i++){
            for(int j = y;j<height;j++){

                double r = pow((double)loadedImage->GetRed(i,j), power);
                double g = pow((double)loadedImage->GetGreen(i,j), power);
                double b = pow((double)loadedImage->GetBlue(i,j), power);
                
                
                double outputRed = Rescale(r, min[0], max[0]);
                double outputGreen = Rescale(g, min[1], max[1]);
                double outputBlue = Rescale(b, min[2], max[2]);
                
                //Set output value
                loadedImage->SetRGB(i,j,outputRed,outputGreen,outputBlue);
            }
        }

        printf(" Finished Power function.\n");
        Refresh();
    }
    else{
        wxMessageBox( wxT("Invalid Input."), wxT("oops!"), wxICON_EXCLAMATION);
    }
}

//Rescales pixel value to 8-bit range
double MyFrame::Rescale(double x, double min, double max){
    
    double oldRange = max - min;
    double newRange = 255 - 0;
    double newValue = (((x - min) * newRange) / oldRange) + 0;
    
    return newValue;
}

void MyFrame::RandomLookupTable(wxCommandEvent & event){
    
    free(loadedImage);
    loadedImage = new wxImage(bitmap.ConvertToImage());
    undoImage = new wxImage(bitmap.ConvertToImage());
    
    map<int, int> lut;
    for(int i =0; i < 256; i++)
    {
        int random = rand() % 255;
        lut.insert(std::pair<int,int>(i,random));
    }   
    for(int i=0; i< imgWidth; i++){

        for(int j=0; j<imgHeight; j++){
            
            int outputRed = lut.at((int)loadedImage->GetRed(i,j));
            int outputGreen = lut.at((int)loadedImage->GetGreen(i,j));
            int outputBlue = lut.at((int)loadedImage->GetBlue(i,j));
                       
            //Set output value
            loadedImage->SetRGB(i,j,outputRed,outputGreen,outputBlue);
        }
    }
    printf(" Finished Lookup table conversion.\n");
    Refresh();
}

double** MyFrame::GetHistogram(wxImage* loadedImage){
    
    double** histogram = 0;
    histogram = new double*[3];
    
    //fill histogram with 0s
    for(int i=0; i<3; i++)
    {
        histogram[i] = new double[256];
        for(int j=0; j<256; j++){
            histogram[i][j] = 0;
        }
    }

    int x = 0;
    int y = 0;
    int width = imgWidth;
    int height = imgHeight;

    if(!selection->IsEmpty()){
        x = selection->GetTopLeft().x;
        y = selection->GetTopLeft().y;
        width = selection->GetBottomRight().x;
        height = selection->GetBottomRight().y;
    }

    //fill Histogram
    for(int i = x;i<width;i++){
        for(int j = y;j<height;j++){
            histogram[0][(int)loadedImage->GetRed(i,j)]++;
            histogram[1][(int)loadedImage->GetGreen(i,j)]++;
            histogram[2][(int)loadedImage->GetBlue(i,j)]++;
        }
    }
    
    return histogram;    
}

double** MyFrame::GetCumulativeHistogram(double** histogram){
    
    double** cumHist = 0;
    cumHist = new double*[3];
    
    //fill histogram with 0s
    for(int i=0; i<3; i++)
    {
        cumHist[i] = new double[256];
        for(int j=0; j<256; j++){
            cumHist[i][j] = 0;
        }
    }
    
    //fill cumulative Histogram
    cumHist[0][0] = histogram[0][0];
    cumHist[1][0] = histogram[1][0];
    cumHist[2][0] = histogram[2][0];
    
    for(int i=1; i< 256; i++){
        for(int j=0; j<3; j++){
            cumHist[j][i] = histogram[j][i] + cumHist[j][i-1];
        }
    }
    
    return cumHist;  
}

void MyFrame::HistogramEqualisation(wxCommandEvent & event){
    
    int sum[3];
    int LUT[3][256];
     
    free(loadedImage);
    loadedImage = new wxImage(bitmap.ConvertToImage());
    undoImage = new wxImage(bitmap.ConvertToImage());
    
    double** histogram = GetHistogram(loadedImage);
    double** cumHist = GetCumulativeHistogram(histogram);
        
    sum[0] = cumHist[0][255];
    sum[1] = cumHist[1][255];
    sum[2] = cumHist[2][255];
    
    //Histogram Normalisation
    for(int i=0; i < 256; i++){
        for(int j=0; j<3; j++){
            histogram[j][i] = histogram[j][i]/sum[j];
            cumHist[j][i] = cumHist[j][i]/sum[j];
        }
    }
    
    //Histogram Equalisation
    for(int j=0; j<3; j++){
        for(int i=0; i<256;i++){
            cumHist[j][i] = cumHist[j][i] * 255;
            LUT[j][i] = static_cast<int>(cumHist[j][i] + 0.5);
        }
    }
    
    int x = 0;
    int y = 0;
    int width = imgWidth;
    int height = imgHeight;

    if(!selection->IsEmpty()){
        x = selection->GetTopLeft().x;
        y = selection->GetTopLeft().y;
        width = selection->GetBottomRight().x;
        height = selection->GetBottomRight().y;
    }

    //Rescale Image   
    for(int i = x;i<width;i++){
        for(int j = y;j<height;j++){
          
            int outputRed = LUT[0][(int)loadedImage->GetRed(i,j)];
            int outputGreen = LUT[1][(int)loadedImage->GetGreen(i,j)];
            int outputBlue = LUT[2][(int)loadedImage->GetBlue(i,j)];
                       
            //Set output value
            loadedImage->SetRGB(i,j,outputRed,outputGreen,outputBlue);
        }
    }
    
    
    printf(" Finished Histogram Equalisation.\n");
    Refresh();   
}

void MyFrame::HistogramStatistics(wxCommandEvent & event){
    double sum[3];
    double mean[3];
    double variance[3];
    
    free(loadedImage);
    loadedImage = new wxImage(bitmap.ConvertToImage());
    undoImage = new wxImage(bitmap.ConvertToImage());

    double** histogram = GetHistogram(loadedImage);
    
    //get sum
    for(int i =0; i<3;i++){
        for(int j=0;j<256;j++){
            sum[i] += histogram[i][j];
        }
    }
    
    //calculate mean
    for(int i=0; i < 3; i++){
        for(int j=0; j<256; j++){
            mean[i] += ((histogram[i][j] * j)/sum[i]);
        }
    }
    
    cout << "Mean" << endl;
    cout << "Red: " << static_cast<int>(mean[0] + 0.5) << endl;
    cout << "Green: " << static_cast<int>(mean[1] + 0.5) << endl;
    cout << "Blue: " << static_cast<int>(mean[2] + 0.5) << endl;
    cout << endl;
    
    int x = 0;
    int y = 0;
    int width = imgWidth;
    int height = imgHeight;

    if(!selection->IsEmpty()){
        x = selection->GetTopLeft().x;
        y = selection->GetTopLeft().y;
        width = selection->GetBottomRight().x;
        height = selection->GetBottomRight().y;
    }
    
    //calculate variance
    for(int i = x;i<width;i++){
        for(int j = y;j<height;j++){
            variance[0] += (pow((int)loadedImage->GetRed(i,j) - mean[0], 2)/sum[0]);
            variance[1] += (pow((int)loadedImage->GetGreen(i,j) - mean[1], 2)/sum[1]);
            variance[2] += (pow((int)loadedImage->GetBlue(i,j) - mean[2], 2)/sum[2]);
        }
    }
       
    cout << "Standard Deviation" << endl;
    cout << "Red: " << static_cast<int>(sqrt(variance[0]) + 0.5) << endl;
    cout << "Green: " << static_cast<int>(sqrt(variance[1]) + 0.5) << endl;
    cout << "Blue: " << static_cast<int>(sqrt(variance[2]) + 0.5) << endl;
    
}

void MyFrame::SimpleThresholding(wxCommandEvent & event){
    
    double threshold = -1;
    
    wxString scaleInput = wxGetTextFromUser (
            wxT("Enter threshold"),
            wxT("Prompt"),
            wxT(""),
            NULL,
            -1, -1, TRUE                
            ); 
    scaleInput.ToDouble(&threshold);
    
    if(threshold >= 0 && threshold < 256){
        
        free(loadedImage);
        loadedImage = new wxImage(bitmap.ConvertToImage());
        undoImage = new wxImage(bitmap.ConvertToImage());

        int x = 0;
        int y = 0;
        int width = imgWidth;
        int height = imgHeight;

        if(!selection->IsEmpty()){
            x = selection->GetTopLeft().x;
            y = selection->GetTopLeft().y;
            width = selection->GetBottomRight().x;
            height = selection->GetBottomRight().y;
        }

        for(int i = x;i<width;i++){
            for(int j = y;j<height;j++){
                
                int outputRed = 0;
                int outputGreen = 0;
                int outputBlue = 0;

                //applies threshold to each individual band of color
//                if((int)loadedImage->GetRed(i,j) >= threshold)
//                    outputRed = loadedImage->GetRed(i,j);
//                if((int)loadedImage->GetGreen(i,j) >= threshold)
//                    outputGreen = loadedImage->GetGreen(i,j);
//                if((int)loadedImage->GetBlue(i,j) >= threshold)
//                    outputBlue = loadedImage->GetBlue(i,j);
                
                //applies threshold on all bands of color, keeps original color
//                if((int)loadedImage->GetRed(i,j) >= threshold ||
//                    (int)loadedImage->GetGreen(i,j) >= threshold ||
//                    (int)loadedImage->GetBlue(i,j) >= threshold)
//                {                    
//                    outputRed = loadedImage->GetRed(i,j);
//                    outputGreen = loadedImage->GetGreen(i,j);
//                    outputBlue = loadedImage->GetBlue(i,j);
//                }
                
                //applies black or white if below or above threshold
                if((int)loadedImage->GetRed(i,j) >= threshold ||
                    (int)loadedImage->GetGreen(i,j) >= threshold ||
                    (int)loadedImage->GetBlue(i,j) >= threshold)
                {                    
                    outputRed = 255;
                    outputGreen = 255;
                    outputBlue = 255;
                }               
                
                //Set output value
                loadedImage->SetRGB(i,j,outputRed,outputGreen,outputBlue);
            }
        }

        printf(" Finished Simple Threshold.\n");
        Refresh();
    }
    else{
        wxMessageBox( wxT("Invalid Input."), wxT("oops!"), wxICON_EXCLAMATION);
    }
}

void MyFrame::AutoThresholding(wxCommandEvent & event){
    
    double sum[3];
    double mean[3];
    
    free(loadedImage);
    loadedImage = new wxImage(bitmap.ConvertToImage());
    undoImage = new wxImage(bitmap.ConvertToImage());

    double** histogram = GetHistogram(loadedImage);

    //get sum
    for(int i =0; i<3;i++){
        for(int j=0;j<256;j++){
            sum[i] += histogram[i][j];
        }
    }
    
    //calculate histogram mean for each colour
    for(int i=0; i < 3; i++){
        for(int j=0; j<256; j++){
            mean[i] += ((histogram[i][j] * j)/sum[i]);
        }
    }
    
    double threshold = 0.0;
    double background;
    double object;
    double totBackground;
    double totObject;
   
    //divides histogram into background or object based on if the average intensity value is above or below the threshold
    //threshold increases until the (mean background value  + mean object value) / 2 == threshold
    while(true){
   
        //Background
        background = 0.0;
        totBackground = 0.0;
        for(int i=0; i <= threshold; i++){
            double avgHistogramValue = (histogram[0][i] + histogram[1][i] + histogram[2][i])/3;
            totBackground += avgHistogramValue;
            background += (avgHistogramValue * i);
        }
        
        //Object
        object = 0.0;
        totObject = 0.0;
        for(int i = threshold + 1; i < 256; i++){
            double avgHistogramValue = (histogram[0][i] + histogram[1][i] + histogram[2][i])/3;
            totObject += avgHistogramValue;
            object += (avgHistogramValue * i);
        }
        
        //finds weighted average object and background gray values and checks if their mean is equal to the threshold.
        if(totBackground > 0 && totObject > 0){
            object /= totObject;
            background /= totBackground;
            if(threshold == static_cast<int>(((object + background)/2.0) + 0.5)){
                
                cout << "object mean gray value: " << object << endl;
                cout << "background mean gray value: " << background << endl;
                break;
            }
                
        }
        threshold++;
        
        if(threshold > 255){
            cout << "Threshold not found" << endl;
            break;
        }
    }
    
    cout << "threshold: " << threshold << endl;
    
    int x = 0;
    int y = 0;
    int width = imgWidth;
    int height = imgHeight;

    if(!selection->IsEmpty()){
        x = selection->GetTopLeft().x;
        y = selection->GetTopLeft().y;
        width = selection->GetBottomRight().x;
        height = selection->GetBottomRight().y;
    }
    
    //applying thresholding to image
    for(int i = x;i<width;i++){
        for(int j = y;j<height;j++){

            int outputRed = 0;
            int outputGreen = 0;
            int outputBlue = 0;

            //applies black or white if below or above threshold
            if((int)loadedImage->GetRed(i,j) >= threshold ||
                (int)loadedImage->GetGreen(i,j) >= threshold ||
                (int)loadedImage->GetBlue(i,j) >= threshold)
            {                    
                outputRed = 255;
                outputGreen = 255;
                outputBlue = 255;
            }               

            //Set output value
            loadedImage->SetRGB(i,j,outputRed,outputGreen,outputBlue);
        }
    }
    
    cout << "Automated Thresholding complete!" << endl;
    Refresh();
}

void MyFrame::Undo(wxCommandEvent & event){
    
    if(undoImage != 0 && undoImage != loadedImage){
        
        loadedImage = undoImage;
        Refresh();
    }
    
}

void MyFrame::OnLeftDown(wxMouseEvent& event){
    //wxRect imageRect = GetImageDisplayRect(GetScrollPosition());
    CaptureMouse();
    Refresh();
    selection = new wxRect(event.GetPosition(), wxSize(0,0));
}

void MyFrame::OnLeftUp(wxMouseEvent& event){
    
    if(HasCapture()){
                
        ReleaseMouse();
        Refresh();
        selection->SetBottomRight(event.GetPosition());
        
        //in case of single click
        if(selection->GetHeight() == 1 && selection->GetWidth() == 1){
            selection->SetSize(wxSize(0,0));
            cout << "selection released" << endl;
        }
        else{
            cout << "selection captured" << endl;
        }
    }
}

void MyFrame::OnMotion(wxMouseEvent& event){
    if(HasCapture()){
        selection->SetBottomRight(event.GetPosition());
        Refresh();
    }
}


//###########################################################//
//-----------------------------------------------------------//
//---------- DO NOT MODIFY THE CODE BELOW--------------------//
//-----------------------------------------------------------//
//###########################################################//


//IMAGE SAVING
void MyFrame::OnSaveImage(wxCommandEvent & event){

    printf("Saving image...");
    free(loadedImage);
    loadedImage = new wxImage(bitmap.ConvertToImage());

    loadedImage->SaveFile(wxT("Saved_Image.bmp"), wxBITMAP_TYPE_BMP);

    printf("Finished Saving.\n");
}


void MyFrame::OnExit (wxCommandEvent & event){
  Close(TRUE);
}


void MyFrame::OnPaint(wxPaintEvent & event){
  wxPaintDC dc(this);
  int cWidth, cHeight;  
  GetSize(&cWidth, &cHeight);
  if ((back_bitmap == NULL) || (oldWidth != cWidth) || (oldHeight != cHeight)) {
    if (back_bitmap != NULL)
      delete back_bitmap;
    back_bitmap = new wxBitmap(cWidth, cHeight);
    oldWidth = cWidth;
    oldHeight = cHeight;
  }
  wxMemoryDC *temp_dc = new wxMemoryDC(&dc);
  temp_dc->SelectObject(*back_bitmap);
  // We can now draw into the offscreen DC...
  temp_dc->Clear();
  if(loadedImage)
    temp_dc->DrawBitmap(wxBitmap(*loadedImage), 0, 0, false);//given bitmap xcoord y coord and transparency
       
  switch(stuffToDraw){
     case NOTHING:
        break;
     case ORIGINAL_IMG:
       bitmap.CleanUpHandlers; // clean the actual image header
       bitmap = wxBitmap(*loadedImage); // Update the edited/loaded image
       break;
  }

// update image size
imgWidth  = (bitmap.ConvertToImage()).GetWidth();
imgHeight = (bitmap.ConvertToImage()).GetHeight();



 temp_dc->SelectObject(bitmap);//given bitmap 
 
 if(selection != NULL && !selection->IsEmpty() && HasCapture()){
    temp_dc->SetBrush(*wxTRANSPARENT_BRUSH);
    temp_dc->SetPen(wxPen(*wxWHITE, 3));
    temp_dc->DrawRectangle(*selection);
 } 

  //end draw all the things
  // Copy from this DC to another DC.
  dc.Blit(0, 0, cWidth, cHeight, temp_dc, 0, 0);
  delete temp_dc; // get rid of the memory DC  
}

BEGIN_EVENT_TABLE (MyFrame, wxFrame)
  EVT_MENU ( LOAD_FILE_ID,  MyFrame::OnOpenFile)
  EVT_MENU ( EXIT_ID,  MyFrame::OnExit)

//###########################################################//
//----------------------START MY EVENTS ---------------------//
//###########################################################// 

  EVT_MENU ( INVERT_IMAGE_ID,  MyFrame::OnInvertImage)
  EVT_MENU ( SHIFT_IMAGE_ID,  MyFrame::OnShiftImage)
  EVT_MENU ( SCALE_IMAGE_ID,  MyFrame::OnScaleImage)
  EVT_MENU ( SAVE_IMAGE_ID,  MyFrame::OnSaveImage)
  EVT_MENU ( LOAD_RAW, MyFrame::OpenRawFile)
  EVT_MENU ( CONVOLUTION_ID, MyFrame::OnConvolution)
  EVT_MENU ( SALT_PEPPER_ID, MyFrame::AddSaltPepper)
  EVT_MENU ( MIN_FILTER_ID, MyFrame::MinFilter)
  EVT_MENU ( MAX_FILTER_ID, MyFrame::MaxFilter)
  EVT_MENU ( MID_POINT_ID, MyFrame::MidPointFilter)
  EVT_MENU ( LOGARITHM_ID, MyFrame::LogTransformation)
  EVT_MENU ( POWER_ID, MyFrame::PowerTransformation)
  EVT_MENU ( NEGATIVE_ID, MyFrame::NegativeLinearTransform)
  EVT_MENU ( LUT_ID, MyFrame::RandomLookupTable)
  EVT_MENU ( HIST_EQ_ID, MyFrame::HistogramEqualisation)
  EVT_MENU ( HIST_STAT_ID, MyFrame::HistogramStatistics)
  EVT_MENU ( SIMPLE_THRESH_ID, MyFrame::SimpleThresholding)
  EVT_MENU ( AUTO_THRESH_ID, MyFrame::AutoThresholding)
  EVT_MENU ( UNDO_ID, MyFrame::Undo)
        
  EVT_LEFT_DOWN( MyFrame::OnLeftDown )
  EVT_LEFT_UP ( MyFrame::OnLeftUp )
  EVT_MOTION (MyFrame::OnMotion)

//###########################################################//
//----------------------END MY EVENTS -----------------------//
//###########################################################// 

  EVT_PAINT (MyFrame::OnPaint)
END_EVENT_TABLE()
