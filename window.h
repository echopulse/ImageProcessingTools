
class BasicApplication : public wxApp {
 public:
    virtual bool OnInit();
};


class MyFrame : public wxFrame {    
private:
    void Convolution(double[][3], bool);

protected:
    wxMenuBar  *menuBar;//main menu bar
    wxMenu     *fileMenu;//file menu
    wxBitmap *back_bitmap; // offscreen memory buffer for drawing
    wxToolBar *toolbar;//toolbar not necessary to use
    int oldWidth, oldHeight; // save old dimensions

    wxBitmap bitmap;  //structure for the edited image
    wxImage *loadedImage; // image loaded from file
    int imgWidth, imgHeight; // image dimensions
    int stuffToDraw; 

   /* declare message handler */
    void OnInvertImage(wxCommandEvent & event);
    void OnScaleImage(wxCommandEvent & event);
    void OnShiftImage(wxCommandEvent & event);
    void OnConvolution(wxCommandEvent & event);
    void OnSaveImage(wxCommandEvent & event);
    void OpenRawFile(wxCommandEvent & event);
    void AddSaltPepper(wxCommandEvent & event);
    void OnMyFunctionImage(wxCommandEvent & event); //---> To be modified!
    
 public:
    MyFrame(const wxString title, int xpos, int ypos, int width, int height);
    virtual ~MyFrame();
    
    void OnExit(wxCommandEvent & event);       
    void OnOpenFile(wxCommandEvent & event);       
    void OnPaint(wxPaintEvent & event);	
    
    DECLARE_EVENT_TABLE()
	
};

enum { NOTHING = 0,
       ORIGINAL_IMG,
};

enum { EXIT_ID = wxID_HIGHEST + 1,      
       LOAD_FILE_ID,
       LOAD_RAW,
       INVERT_IMAGE_ID,
       SCALE_IMAGE_ID,
       CONVOLUTION_ID,
       SHIFT_IMAGE_ID,
       SAVE_IMAGE_ID,
       SALT_PEPPER_ID,
       MY_IMAGE_ID, //--->To be modified!
};
