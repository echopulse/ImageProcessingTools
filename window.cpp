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
  fileMenu->Append(LOAD_FILE_ID, _T("&Open file"));
  fileMenu->Append(LOAD_RAW, _T("&Open Raw File"));  
  fileMenu->AppendSeparator();
//###########################################################//
//----------------------START MY MENU -----------------------//
//###########################################################// 
  
  fileMenu->Append(INVERT_IMAGE_ID, _T("&Invert image"));
  fileMenu->Append(SCALE_IMAGE_ID, _T("&Scale image"));
  fileMenu->Append(SHIFT_IMAGE_ID, _T("&Shift image"));
  fileMenu->Append(CONVOLUTION_ID, _T("&Convolution"));
  fileMenu->Append(MY_IMAGE_ID, _T("&My function")); //--->To be modified!
 
//###########################################################//
//----------------------END MY MENU -------------------------//
//###########################################################// 

  fileMenu->AppendSeparator();
  fileMenu->Append(SAVE_IMAGE_ID, _T("&Save image"));
  fileMenu->Append(EXIT_ID, _T("&Exit"));

  menuBar = new wxMenuBar;
  menuBar->Append(fileMenu, _T("&File"));
  
  SetMenuBar(menuBar);
  CreateStatusBar(3); 
  oldWidth = 0;
  oldHeight = 0;
  loadedImage = 0;

/* initialise the variables that we added */
  imgWidth = imgHeight = 0;
  stuffToDraw = 0;

}

MyFrame::~MyFrame(){
/* release resources */
  if(loadedImage){
    loadedImage->Destroy();
    loadedImage = 0;
  }

}

void MyFrame::OnOpenFile(wxCommandEvent & event){
  wxFileDialog *openFileDialog = new wxFileDialog ( this, _T("Open file"), _T(""), _T(""), FILETYPES, wxOPEN, wxDefaultPosition);  
  if(openFileDialog->ShowModal() == wxID_OK){
    wxString filename = openFileDialog->GetFilename();
    wxString path = openFileDialog->GetPath();
    printf("Loading image from file...");    
    loadedImage = new wxImage(path); //Image Loaded form file 
    if(loadedImage->Ok()){
      stuffToDraw = ORIGINAL_IMG;    // set the display flag
      printf("Done! \n");    
    }
    else {
      printf("error:...");
      loadedImage->Destroy();
      loadedImage = 0;
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
void MyFrame::openRawFile(wxCommandEvent & event){

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
        }
        
        Refresh();
    }
}


//INVERT IMAGE
void MyFrame::OnInvertImage(wxCommandEvent & event){
  
    printf("Inverting...");
    free(loadedImage);
    loadedImage = new wxImage(bitmap.ConvertToImage());

    for( int i=0;i<imgWidth;i++)
       for(int j=0;j<imgHeight;j++){
 	loadedImage->SetRGB(i,j,255-loadedImage->GetRed(i,j), 
				255-loadedImage->GetGreen(i,j),
				255-loadedImage->GetBlue(i,j));
    }

    printf(" Finished inverting.\n");
    Refresh();
}

//IMAGE SCALING
void MyFrame::OnScaleImage(wxCommandEvent & event){

    printf("Scaling...");
    free(loadedImage);
    loadedImage = new wxImage(bitmap.ConvertToImage());

    for( int i=0;i<imgWidth;i++)
       for(int j=0;j<imgHeight;j++){
 	loadedImage->SetRGB(i,j,2.5* loadedImage->GetRed(i,j), 
				2.5* loadedImage->GetGreen(i,j),
				2.5* loadedImage->GetBlue(i,j));
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

        for( int i=0;i<imgWidth;i++)
           for(int j=0;j<imgHeight;j++){
            
               
               
               
            loadedImage->SetRGB(i,j,max(min(shift + loadedImage->GetRed(i,j), 255.0), 0.0), 
                                    max(min(shift + loadedImage->GetGreen(i,j), 255.0), 0.0),
                                    max(min(shift + loadedImage->GetBlue(i,j), 255.0), 0.0)
                        );
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
            wxT("Enter number corresponding to mask:\n 1. Averaging\n 2. Weighted Averaging\n"
            "3. 4-neighbour Laplacian\n 4. 8-neighbour Laplacian\n 5. 4-neighbour Laplacian Enhancement\n "
            "6. 8-neighbour Laplacian Enhancement\n 7. Roberts\n 8. Sobel X\n 9. Sobel Y"),
            wxT("Prompt"),
            wxT(""),
            NULL,
            -1, -1, TRUE                
            ); 
    double input;
    scaleInput.ToDouble(&input);
    int selection = static_cast<int>(input + 0.5);
    
    if(selection > 9 || selection < 1)
    {
        wxMessageBox( wxT("Invalid Input."), wxT("oops!"), wxICON_EXCLAMATION);
    }
    else
    {
        switch(selection)
        {
            double mask [3][3];
            case 1 :
                for(int i = 0; i < 3; i++)
                {
                    for(int j = 0; j < 3; j++)
                    {
                        mask[i][j] = 1.0/9.0;
                    }
                }
                MyFrame::Convolution(mask);
                break;
            case 2 :
                break;
            case 3 :
               break;
            case 4 :
               break;
            case 5 :
               break;
            case 6 :
                break;
            case 7 :
                break;
            case 8:
                break;
            case 9:
                break;
            default :
                wxMessageBox( wxT("Invalid Input."), wxT("oops!"), wxICON_EXCLAMATION);
        }
        cout << "Finished convolution";
        Refresh();
    }
    
}

//Convolution Function
void MyFrame::Convolution(double mask[3][3])
{
    for (int m=0; m<3; m++)
    {
        for(int n=0; n<3; n++)
        {
                cout << mask[m][n] << ' ';
        }
        cout << '\n';
    }
    
    free(loadedImage);
    loadedImage = new wxImage(bitmap.ConvertToImage());
    
    for(int i=0;i< imgWidth; i++)
    {
        for(int j=0;j<imgHeight;j++)
        {                 
            //Compute average
            double averageRed = 0.0;
            double averageBlue = 0.0;
            double averageGreen = 0.0;
            
            //RED
            //row 0
            averageRed += mask[0][0] * loadedImage->GetRed(i-1,j-1);
            averageRed += mask[0][1] * loadedImage->GetRed(i,j-1);
            averageRed += mask[0][2] * loadedImage->GetRed(i+1,j-1);
            //row 1
            averageRed += mask[1][0] * loadedImage->GetRed(i-1,j);
            averageRed += mask[1][1] * loadedImage->GetRed(i,j);
            averageRed += mask[1][2] * loadedImage->GetRed(i+1,j);
            //row 2
            averageRed += mask[2][0] * loadedImage->GetRed(i-1,j+1);
            averageRed += mask[2][1] * loadedImage->GetRed(i,j+1);
            averageRed += mask[2][2] * loadedImage->GetRed(i+1,j+1);
            
            //BLUE
            //row 0
            averageBlue += mask[0][0] * loadedImage->GetBlue(i-1,j-1);
            averageBlue += mask[0][1] * loadedImage->GetBlue(i,j-1);
            averageBlue += mask[0][2] * loadedImage->GetBlue(i+1,j-1);
            //row 1
            averageBlue += mask[1][0] * loadedImage->GetBlue(i-1,j);
            averageBlue += mask[1][1] * loadedImage->GetBlue(i,j);
            averageBlue += mask[1][2] * loadedImage->GetBlue(i+1,j);
            //row 2
            averageBlue += mask[2][0] * loadedImage->GetBlue(i-1,j+1);
            averageBlue += mask[2][1] * loadedImage->GetBlue(i,j+1);
            averageBlue += mask[2][2] * loadedImage->GetBlue(i+1,j+1);
            
            //GREEN
            //row 0
            averageGreen += mask[0][0] * loadedImage->GetGreen(i-1,j-1);
            averageGreen += mask[0][1] * loadedImage->GetGreen(i-1,j-1);
            averageGreen += mask[0][2] * loadedImage->GetGreen(i-1,j-1);
            //row 1
            averageGreen += mask[1][0] * loadedImage->GetGreen(i-1,j-1);
            averageGreen += mask[1][1] * loadedImage->GetGreen(i-1,j-1);
            averageGreen += mask[1][2] * loadedImage->GetGreen(i-1,j-1);
            //row 2
            averageGreen += mask[2][0] * loadedImage->GetGreen(i-1,j-1);
            averageGreen += mask[2][1] * loadedImage->GetGreen(i-1,j-1);
            averageGreen += mask[2][2] * loadedImage->GetGreen(i-1,j-1);
            
            //Set output value
            loadedImage->SetRGB(i,j,averageRed,averageBlue,averageGreen);
        }
    }
}


//My Function ---> To be modified!
void MyFrame::OnMyFunctionImage(wxCommandEvent & event){

    printf("My function...");
    free(loadedImage);
    loadedImage = new wxImage(bitmap.ConvertToImage());

    unsigned char r,g,b;

    for( int i=0;i<imgWidth;i++)
       for(int j=0;j<imgHeight;j++){
	// GET THE RGB VALUES 	
        r = loadedImage->GetRed(i,j);   // red pixel value
        g = loadedImage->GetGreen(i,j); // green pixel value
	b = loadedImage->GetBlue(i,j); // blue pixel value

	printf("(%d,%d) [r = %x  | g = %x | b = %x] \n",i,j,r,g,b);        
        
	// SAVE THE RGB VALUES
	loadedImage->SetRGB(i,j,r,g,b); 
    }
	
    printf(" Finished My function.\n");
    Refresh();
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
  EVT_MENU ( LOAD_RAW, MyFrame::openRawFile)
  EVT_MENU ( CONVOLUTION_ID, MyFrame::OnConvolution)
  EVT_MENU ( MY_IMAGE_ID,  MyFrame::OnMyFunctionImage)//--->To be modified!

//###########################################################//
//----------------------END MY EVENTS -----------------------//
//###########################################################// 

  EVT_PAINT (MyFrame::OnPaint)
END_EVENT_TABLE()
