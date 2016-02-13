using System;
using System.Collections.Generic;
using System.IO;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Windows.Forms;
using System.Drawing.Drawing2D;


namespace TrainingCrt
{
    public partial class Form1 : Form
    {
        #region Properties
        
        bool brushing = false,lineDown = false;
        int fx = 0,fy = 0,dox = 0,doy = 0;
        
        int penWidth = 1;
        int bmode = 1;//1-draw,0-erase
        int mode = 1; //1-brush/eraser,3-line,4-fill
        List<string> files = new List<string>();
        static List<string> ImageExtensions = new List<string> { ".JPG", "JPEG", ".JPE", ".BMP", ".PNG" };
        int curfile=-1;
        DrawStuff ds = new DrawStuff();
        DataStuff da = new DataStuff();
        
        #endregion

        #region Init

        public Form1()
        {
        	
            InitializeComponent();
        }

        #endregion

        #region On Painting

        protected override void OnPaint(PaintEventArgs e)
        {
            Graphics g = e.Graphics;            
            g.SmoothingMode = SmoothingMode.AntiAlias;
            g.InterpolationMode = InterpolationMode.NearestNeighbor;
            g.PixelOffsetMode = PixelOffsetMode.Half;
           
            if(curfile >-1){
            	ds.DrawImage(files[curfile],g,this,flowLayoutPanel1);
            	ds.DrawMask(g);
            }
            Cursor.Current = Cursors.Default;
                        
        }

        #endregion

        #region Mouse

        protected override void OnMouseDown(MouseEventArgs e)
        {
            
            
            if (e.Button == System.Windows.Forms.MouseButtons.Left)
            {
            	if(mode == 4){//fill
            		Cursor.Current = Cursors.WaitCursor;
            		createUndo();
            		ds.Fill(e.X,e.Y,bmode,ref da.mask);
            		
            	}
            	else if(mode == 3){//line
            		if(!lineDown){
            			lineDown = true;
            			fx = e.X;
            			fy = e.Y;            				
            				
            		}
            		else{            			
            			//clear overlay
		            	ds.clr = ds.nope;
		            	ds.doDot(fx,fy,penWidth,ds.overlay,false,ref da.mask,false);
            			ds.doDot(dox,doy,penWidth,ds.overlay,false,ref da.mask,false);
		            	ds.RasterizeLine(fx,fy,dox,doy,1,ds.overlay,false,ref da.mask);
		            	if(bmode == 1)
		            		ds.clr = ds.yes;
		            	else
		            		ds.clr = ds.nope;
            			
            			Cursor.Current = Cursors.WaitCursor;
            			createUndo();
            			ds.RasterizeLine(fx,fy,dox,doy,penWidth,ds.bitmask,true,ref da.mask);
            			lineDown = false;
            		}
            	}
            	else if(mode == 1){//brush/eraser
            		//clear overlay
	            	ds.clr = ds.nope;
	            	ds.doDot(dox,doy,penWidth,ds.overlay,false,ref da.mask,false);
	            	if(bmode == 1)
	            		ds.clr = ds.yes;
	            	else
	            		ds.clr = ds.eras;
            		brushing = true;
            		if(bmode == 1)
		            	ds.clr = ds.yes;
	            	else
	            		ds.clr = ds.nope;
	            	createUndo();
            		ds.doDot(dox,doy,penWidth,ds.bitmask,true,ref da.mask,true);
            	}
            	updateName(false);
            }
            Invalidate();
        }

        protected override void OnMouseUp(MouseEventArgs e)
        {
        	brushing = false;
            Invalidate();
        }

        protected override void OnMouseMove(MouseEventArgs e)
        {
            if (lineDown)
            {
            	//clear overlay
            	ds.clr = ds.nope;
            	ds.doDot(fx,fy,penWidth,ds.overlay,false,ref da.mask,false);
            	ds.RasterizeLine(fx,fy,dox,doy,1,ds.overlay,false,ref da.mask);
            	ds.doDot(dox,doy,penWidth,ds.overlay,false,ref da.mask,false);
            	if(bmode == 1)
            		ds.clr = ds.yes;
            	else
            		ds.clr = ds.eras;
            	dox = e.X;
            	doy = e.Y;
            	
            	ds.doDot(fx,fy,penWidth,ds.overlay,false,ref da.mask,false);
            	ds.RasterizeLine(fx,fy,dox,doy,1,ds.overlay,false,ref da.mask);
            	ds.doDot(dox,doy,penWidth,ds.overlay,false,ref da.mask,false);
                
            }
            else if(mode !=4){
            	if(brushing){
            		if(bmode == 1)
		            	ds.clr = ds.yes;
	            	else
	            		ds.clr = ds.nope;
            		ds.doDot(e.X,e.Y,penWidth,ds.bitmask,true,ref da.mask,true);
            	}
            	else{
	            	//clear overlay
	            	ds.clr = ds.nope;
	            	ds.doDot(dox,doy,penWidth,ds.overlay,false,ref da.mask,false);
	            	if(bmode == 1)
	            		ds.clr = ds.yes;
	            	else
	            		ds.clr = ds.eras;
	            	dox = e.X;
	            	doy = e.Y;
	            	ds.doDot(dox,doy,penWidth,ds.overlay,false,ref da.mask,false);
	            }
            	
            }
            Invalidate();
        }

        #endregion
        
        #region Buttons
        
        void undo_click(object sender, EventArgs e)
        {
        	undo();
        }
        
        
        void next_click(object sender, EventArgs e)
        {
        	next();
        }
        
        void prev_click(object sender, EventArgs e)
        {
        	prev();
        }
        
        void sel_folder_click(object sender, EventArgs e)
        {
        	FolderBrowserDialog fbd = new FolderBrowserDialog();
			DialogResult result = fbd.ShowDialog();
			
			string[] all = Directory.GetFiles(fbd.SelectedPath);
			//remove teaching files by extension
			files = new List<string>();
			foreach(string s in all){
				if (ImageExtensions.Contains(Path.GetExtension(s).ToUpperInvariant())){
					files.Add(s);
				}
			}
			if(files.Count >0){
				curfile = 0;			
				da.createMatrix(files[curfile]);
				ds.UpdateMask(ref da.mask,ref da.floatymask);
			}
			updateName(true);
			Invalidate();
			
        }
                
        
        void save_click(object sender, EventArgs e)
        {
        	lineDown = false;        	
        	save();
        }
                
        
        void trackbar_change(object sender, EventArgs e)
        {
        	penWidth = trackBar1.Value;
        	lineDown = false;
        }
        
        void line_click(object sender, EventArgs e)
        {
        	mode = 3;
        	lineDown = false;
        }
        
        void brush_click(object sender, EventArgs e)
        {
        	bmode = 1;
        	mode = 1;
        	ds.clr = ds.yes;
        	lineDown = false;
        }               
        
        void eraser_click(object sender, EventArgs e)
        {
        	bmode = 0;
        	mode = 1;
        	ds.clr = ds.nope;
        	lineDown = false;
        }
        
        void fill_click(object sender, EventArgs e)
        {
        	ds.clr = ds.nope;
        	if(bmode==1)
        		ds.clr = ds.yes;
        	mode = 4;
        	lineDown = false;
        }
                
       
        void KeyDown_handl(object sender, KeyEventArgs e)
        {
        	if (e.KeyCode == Keys.S && e.Modifiers == Keys.Control){
        		save();
        	}
        	else if (e.KeyCode == Keys.Z && e.Modifiers == Keys.Control){
        		undo();
        	}
        	else if (e.KeyCode == Keys.N){
        		next();
        	}
        	else if (e.KeyCode == Keys.M){
        		prev();
        	}
        }
         #endregion
        
        #region Functions
        
        
        void clrOverlay(){
        	ds.overlay = new Bitmap(ds.overlay.Width,ds.overlay.Height);
        	Invalidate();
        }
         
        void save(){
        	Cursor.Current = Cursors.WaitCursor;
        	clrOverlay();
        	if(curfile > -1 && curfile < files.Count && files[curfile].Length > 0)
        		da.save(files[curfile]);
        	updateName(true);
        	Cursor.Current = Cursors.Default;
        }
        
        void undo(){
        	Cursor.Current = Cursors.WaitCursor;
        	da.applyUndo();
        	ds.UpdateMask(ref da.mask,ref da.floatymask);
        	updateName(false);
        	Invalidate();
        	Cursor.Current = Cursors.Default;
        }
        
        void next(){
        	if(files.Count < 1)
        		return;
        	curfile++;
        	if(curfile == files.Count)
        		curfile = 0;
        	da.createMatrix(files[curfile]);
        	ds.UpdateMask(ref da.mask,ref da.floatymask);
        	updateName(true);
        	Invalidate();
        }
        
        void prev(){
        	if(files.Count < 1)
        		return;
        	curfile--;
        	if(curfile == -1)
        		curfile = files.Count-1;
        	da.createMatrix(files[curfile]);
        	ds.UpdateMask(ref da.mask,ref da.floatymask);
        	updateName(true);
        	Invalidate();
        }
        
        void createUndo(){
        	da.saveForUndo();
        }
        
        void updateName(bool saved){
        	String nm = "NO IMAGES FOUND";
        	if(files.Count >0)
        		nm = files[curfile];
        	
        	if(!saved){
        		nm = nm + "*";
        	}
        	this.Text = nm;
        }
        #endregion
         
    }
}
