/*
 * Created by SharpDevelop.
 * User: Mikulas
 * Date: 03/05/2014
 * Time: 16:27
 * 
 * To change this template use Tools | Options | Coding | Edit Standard Headers.
 */
using System;
using System.IO;
using System.Drawing;

namespace TrainingCrt
{
	/// <summary>
	/// Description of Class1.
	/// </summary>
	public class DataStuff
	{
		public int[,] mask;
		public float[,] floatymask;
		private int[,] undobuf;
		public DataStuff()
		{
		}
		//load
		public void createMatrix(string file){
			//get image for dimensions
			Image im = Image.FromFile(file);
			if(im.Width < 1 || im.Height < 1)
				return;
			mask = new int[im.Width,im.Height];
			floatymask = new float[im.Width,im.Height];
			//try to load previously created
			if(!File.Exists(file + ".trn"))
				return;
			string[] lines = File.ReadAllLines(file + ".trn");
			string[] a = lines[0].Split(' ');
			int n = 0;
			float mnam;
			if(lines[0].Length >0){
				for(int j = 0; j< im.Height;j++){
					for(int i = 0; i< im.Width;i++){
						if(n < a.Length){
							float.TryParse(a[n],out mnam);
							floatymask[i,j] = mnam;
						}
						if(n < a.Length && a[n].Length > 0 && (a[n][0] == '1' || (a[n].Length > 2 && (a[n][2] == '5' ||a[n][2] == '6' ||a[n][2] == '7' ||a[n][2] == '8' ||a[n][2] == '9' ))))
							mask[i,j] = 1;
						
						n++;
					}
				}
			}
		}
		
		public void save(string name){
			using (System.IO.StreamWriter file = new System.IO.StreamWriter((name + ".trn")))
	        {
				for(int j = 0; j< mask.GetLength(1);j++){
					for(int i = 0; i< mask.GetLength(0);i++){					
						file.Write(mask[i,j].ToString() + " ");
					}
				}	                
	            
	        }
		}
		public void saveForUndo(){
			undobuf = (int[,])mask.Clone();
		}
		
		public void applyUndo(){
			int[,] pom = (int[,])mask.Clone();
			mask = (int[,])undobuf.Clone();
			undobuf = pom;
		}
	}
}
