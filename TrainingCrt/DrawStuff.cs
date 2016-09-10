/*
 * Created by SharpDevelop.
 * User: Mikulas
 * Date: 03/05/2014
 * Time: 16:10
 * 
 * To change this template use Tools | Options | Coding | Edit Standard Headers.
 */
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;

namespace TrainingCrt
{
	public class DrawStuff
	{
		public Bitmap bitmask = new Bitmap(1,1);
		public Bitmap overlay = new Bitmap(1,1);
		
		public Color yes = Color.FromArgb(127, 255, 0, 0);
		public Color eras = Color.FromArgb(180, 0, 255, 0);
		public Color nope = Color.FromArgb(0, 255, 255, 255);
			
		public Color clr = Color.FromArgb(127, 255, 0, 0);
		
		public DrawStuff()
		{
			
		}
		
		public void DrawImage(string file, Graphics g,Form f,FlowLayoutPanel panel){

			Image im = Image.FromFile(file);
			f.Width = im.Width+panel.Width;
            panel.Height = Math.Max(450, im.Height);
			f.Height = panel.Height+ SystemInformation.CaptionHeight +30*SystemInformation.BorderSize.Height;
			panel.Location = new Point(im.Width,0);
			g.DrawImage(im, new Point(0, 0));
			
			
		}
		
		public void DrawMask(Graphics g){
			g.DrawImage(bitmask,new Point(0,0));
			g.DrawImage(overlay,new Point(0,0));
		}
		//updatne cely mask podla dat
		public void UpdateMask(ref int[,] mask, ref float[,] floatymask){
			bool usefloat = false;                                       //TODO sprav checkbox ci pouzivat floaty al ne
			
			bitmask = new Bitmap(mask.GetLength(0),mask.GetLength(1));
			overlay = new Bitmap(mask.GetLength(0),mask.GetLength(1));
			
			for(int i = 0; i< mask.GetLength(0);i++){
				for(int j = 0; j< mask.GetLength(1); j++){					
					if(usefloat){
						bitmask.SetPixel(i,j,Color.FromArgb((int)(127*(floatymask[i,j])), 255, 0, 0));
					}
					else if(mask[i,j] == 1)
							bitmask.SetPixel(i,j,yes);
					
						
				}
			}
		}
		public void Fill(int x,int y,int fill,ref int[,] msk)
        {
			if(x < 0 || y < 0 || x >= bitmask.Width || y >= bitmask.Height )
        		return;
			if(fill == msk[x,y])
				return;
			
			Queue<Point> q = new Queue<Point>();
			q.Enqueue(new Point(x,y));
			Point pom;
			while(q.Count > 0){
				pom = q.Dequeue();
					if(msk[pom.X,pom.Y] != fill){
						msk[pom.X,pom.Y] = fill;
						bitmask.SetPixel(pom.X,pom.Y,clr);
						//podmienka rozdrobena pre efektivitu
						if(pom.X+1 < msk.GetLength(0) && msk[pom.X+1,pom.Y] != fill)
							q.Enqueue(new Point(pom.X+1,pom.Y));
						if(pom.Y+1 < msk.GetLength(1) && msk[pom.X,pom.Y+1] != fill)
							q.Enqueue(new Point(pom.X,pom.Y+1));
						if(pom.X > 0 && msk[pom.X-1,pom.Y] != fill)
							q.Enqueue(new Point(pom.X-1,pom.Y));
						if(pom.Y > 0 && msk[pom.X,pom.Y-1] != fill)
							q.Enqueue(new Point(pom.X,pom.Y-1));
					}
				
			}
			
        }
		
		
		
		
		
		public void RasterizeLine(int x0, int y0, int x1, int y1, int brushwidth,Bitmap b,bool maskit,ref int[,] msk)
        {
            // Choose which oktant - first X or second Y
            if (Math.Abs(x1 - x0) > Math.Abs(y1 - y0))
                RasterizeLineBresenhamX(x0, y0, x1, y1,brushwidth,b,maskit,ref msk);
            else
                RasterizeLineBresenhamY(x0, y0, x1, y1,brushwidth,b,maskit,ref msk);
        }
		
		protected void RasterizeLineBresenhamX(int x0, int y0, int x1, int y1,int brushwidth,Bitmap b,bool maskit,ref int[,] msk)
        {
            int dx = x1 - x0;
            int dy = y1 - y0;
            int sx = (dx > 0) ? 1 : -1;
            int sy = (dy > 0) ? 1 : -1;
            dx = Math.Abs(dx);
            dy = Math.Abs(dy);

            
            int c1 = 2 * dy;
            int c2 = 2 * (dy - dx);
            int p = c1 - dx;

            doDot(x0,y0,brushwidth,b,maskit,ref msk,true);
            while (x0 != x1)
            {
                x0 += sx;
                if (p < 0)
                {
                    p += c1;
                }
                else
                {
                    p += c2;
                    y0 += sy;
                }
                doDot(x0,y0,brushwidth,b,maskit,ref msk,true);
            }
        }

        protected void RasterizeLineBresenhamY(int x0, int y0, int x1, int y1,int brushwidth,Bitmap b,bool maskit,ref int[,] msk)
        {
            int dx = x1 - x0;
            int dy = y1 - y0;
            int sx = (dx > 0) ? 1 : -1;
            int sy = (dy > 0) ? 1 : -1;
            dx = Math.Abs(dx);
            dy = Math.Abs(dy);

            int c1 = 2 * dx;
            int c2 = 2 * (dx - dy);
            int p = c1 - dy;

            doDot(x0,y0,brushwidth,b,maskit,ref msk,true);
            while (y0 != y1)
            {
                y0 += sy;
                if (p < 0)
                {
                    p += c1;
                }
                else
                {
                    p += c2;
                    x0 += sx;
                }
                doDot(x0,y0,brushwidth,b,maskit,ref msk,true);
            }
        }
        
        public void doDot(int x,int y,int radius,Bitmap b,bool maskit,ref int[,] msk,bool fill){
        	
        	if(radius == 1){
	        	
	        	if(x < b.Width && y < b.Height && x > -1 && y > -1){
		        	b.SetPixel(x,y,clr);
		        	if(maskit){
		        		if(clr == yes)        			
		        			msk[x,y] = 1;
		        		else
		        			msk[x,y] = 0;
		        	}
	        	}
        	}
        	else{
        		MidpointCircle(radius,x,y,maskit,ref msk,b,fill);
        	}
        	
        }
        
        
        
        public void MidpointCircle(Int32 r, Int32 x0, Int32 y0, bool maskit,ref int[,] msk,Bitmap b,bool fill)
        {
            //Midpoint algo s vyplnou
            
            
            int d = 1- r;
            int curx = r;//current x
            int cury = 0;
            
            while(cury <= curx){
            	//octanty
            	
            	if(fill){
            	//liny medzi bodmi v oktantoch
            	midpointline(x0 - cury,x0 + cury, y0 + curx,maskit, ref msk,b);
            	
            	midpointline(x0 - curx,x0 + curx, y0 + cury,maskit, ref msk,b);
            	           	
            	midpointline(x0 - curx,x0 + curx, y0 - cury,maskit, ref msk,b);
            	            	
            	midpointline(x0 - cury,x0 + cury, y0 - curx,maskit, ref msk,b);
            	}
            	else{
            		pomocnesetpixel(b,x0 + cury,y0+curx,ref msk,maskit);
            		pomocnesetpixel(b,x0 - cury,y0+curx,ref msk,maskit);
	            	
            		pomocnesetpixel(b,x0 + curx,y0+cury,ref msk,maskit);
            		pomocnesetpixel(b,x0 - curx,y0+cury,ref msk,maskit);
	            	
					pomocnesetpixel(b,x0 + curx,y0-cury,ref msk,maskit);
					pomocnesetpixel(b,x0 - curx,y0-cury,ref msk,maskit);	            	
	            	
					pomocnesetpixel(b,x0 + cury,y0-curx,ref msk,maskit);
					pomocnesetpixel(b,x0 - cury,y0-curx,ref msk,maskit);					
	            	
            	}
            	cury++;
            	            	
            	if(d < 0){
            		d += 2*cury+1;
            	}
            	else{
            		curx--;	
            		d += 2*(cury - curx +1);
            	}
            	
            	
            	
            	
            }
          
        }
        
        private void pomocnesetpixel(Bitmap b,int x, int y,ref int[,] msk,bool maskit){
        	if(x < 0 || y < 0 || x >= b.Width || y >= b.Height )
        		return;
        	b.SetPixel(x,y,clr);
        	if(maskit){
        		int pem=0;
				if(clr == yes){
        			pem = 1;
				}
        		msk[x,y] = pem;
        	}
        }
        
        private void midpointline(int from, int to, int y,bool maskit,ref int[,] msk,Bitmap b){
        	if(from > to){
        		int pom = from;
        		from = to;
        		to = pom;
        	}
        	for(int i = from; i <= to;i++){
        		if(i < 0 || i >= b.Width || y < 0 || y >= b.Height)
        			continue;
        		b.SetPixel(i,y,clr);
        		if(maskit){
	        		if(clr == yes)        			
	        			msk[i,y] = 1;
	        		else
	        			msk[i,y] = 0;
	        	}
        	}
        }
		
		
		
		
		
		
	}
}
