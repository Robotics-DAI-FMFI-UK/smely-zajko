using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Drawing;

namespace CG1.Ex07.Rasterization
{
	class Line
	{
		
		public void RasterizeLineBresenham(Graphics g, int x0, int y0, int x1, int y1, Int32 CellSize)
		{						
			
			int sirka = x1 - x0 ;
			int vyska = y1 - y0 ;
			
			//vsetky octanty
			int ox1 = 0;
			int oy1 = 0;
			int ox2 = 0;
			int oy2 = 0 ;
			//zisti v ktorom octante(al jeho mirrore to sa opravi checkom dlhsia<=kratka nizsie) sa nachadzame
			if(sirka<0){
				ox1 = -1 ; 
				ox2 = -1 ;
			}
			else if(sirka>0){
				ox1 = 1 ;
				ox2 = 1 ;
			}
			
			if(vyska<0){
				oy1 = -1 ;
			}
			else if(vyska>0){
				oy1 = 1 ;
			}
			
			//najdi ktorou osou je dlhsia
			int dlhsia = Math.Abs(sirka);
			int kratka = Math.Abs(vyska);	
			
			if(dlhsia <= kratka){
				dlhsia = Math.Abs(vyska);
				kratka = Math.Abs(sirka);
				if(vyska<0) 
					oy2 = -1;
				else if(vyska>0) 
					oy2 = 1;
				ox2 = 0;            
			}
			
			int krok = dlhsia/2 ;
			
			for (int i=0; i <= dlhsia; i++){
				g.FillRectangle(Brushes.Black,x0*CellSize,y0*CellSize,CellSize,CellSize);
				krok += kratka;
				if (krok < dlhsia){
					x0 += ox2;
					y0 += oy2;
				} else{
					krok -= dlhsia;
					x0 += ox1;
					y0 += oy1;
				}		   
			}
		
		
		
		}
		
		
	}
}
