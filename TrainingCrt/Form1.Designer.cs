namespace TrainingCrt
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
        	this.groupBox1 = new System.Windows.Forms.GroupBox();
        	this.button9 = new System.Windows.Forms.Button();
        	this.button7 = new System.Windows.Forms.Button();
        	this.trackBar1 = new System.Windows.Forms.TrackBar();
        	this.button3 = new System.Windows.Forms.Button();
        	this.button2 = new System.Windows.Forms.Button();
        	this.button1 = new System.Windows.Forms.Button();
        	this.flowLayoutPanel1 = new System.Windows.Forms.FlowLayoutPanel();
        	this.groupBox2 = new System.Windows.Forms.GroupBox();
        	this.button8 = new System.Windows.Forms.Button();
        	this.button4 = new System.Windows.Forms.Button();
        	this.button5 = new System.Windows.Forms.Button();
        	this.button6 = new System.Windows.Forms.Button();
        	this.groupBox1.SuspendLayout();
        	((System.ComponentModel.ISupportInitialize)(this.trackBar1)).BeginInit();
        	this.flowLayoutPanel1.SuspendLayout();
        	this.groupBox2.SuspendLayout();
        	this.SuspendLayout();
        	// 
        	// groupBox1
        	// 
        	this.groupBox1.Controls.Add(this.button9);
        	this.groupBox1.Controls.Add(this.button7);
        	this.groupBox1.Controls.Add(this.trackBar1);
        	this.groupBox1.Controls.Add(this.button3);
        	this.groupBox1.Controls.Add(this.button2);
        	this.groupBox1.Controls.Add(this.button1);
        	this.groupBox1.Location = new System.Drawing.Point(3, 3);
        	this.groupBox1.Name = "groupBox1";
        	this.groupBox1.Size = new System.Drawing.Size(144, 285);
        	this.groupBox1.TabIndex = 4;
        	this.groupBox1.TabStop = false;
        	this.groupBox1.Text = "Draw";
        	// 
        	// button9
        	// 
        	this.button9.Location = new System.Drawing.Point(6, 135);
        	this.button9.Name = "button9";
        	this.button9.Size = new System.Drawing.Size(88, 23);
        	this.button9.TabIndex = 5;
        	this.button9.Text = "Line";
        	this.button9.UseVisualStyleBackColor = true;
        	this.button9.Click += new System.EventHandler(this.line_click);
        	// 
        	// button7
        	// 
        	this.button7.Location = new System.Drawing.Point(6, 106);
        	this.button7.Name = "button7";
        	this.button7.Size = new System.Drawing.Size(88, 23);
        	this.button7.TabIndex = 4;
        	this.button7.Text = "Undo(CTRL+Z)";
        	this.button7.UseVisualStyleBackColor = true;
        	this.button7.Click += new System.EventHandler(this.undo_click);
        	// 
        	// trackBar1
        	// 
        	this.trackBar1.Location = new System.Drawing.Point(95, 19);
        	this.trackBar1.Maximum = 20;
        	this.trackBar1.Minimum = 1;
        	this.trackBar1.Name = "trackBar1";
        	this.trackBar1.Orientation = System.Windows.Forms.Orientation.Vertical;
        	this.trackBar1.Size = new System.Drawing.Size(42, 260);
        	this.trackBar1.TabIndex = 3;
        	this.trackBar1.Value = 1;
        	this.trackBar1.ValueChanged += new System.EventHandler(this.trackbar_change);
        	// 
        	// button3
        	// 
        	this.button3.Location = new System.Drawing.Point(6, 77);
        	this.button3.Name = "button3";
        	this.button3.Size = new System.Drawing.Size(88, 23);
        	this.button3.TabIndex = 2;
        	this.button3.Text = "Fill";
        	this.button3.UseVisualStyleBackColor = true;
        	this.button3.Click += new System.EventHandler(this.fill_click);
        	// 
        	// button2
        	// 
        	this.button2.Location = new System.Drawing.Point(6, 48);
        	this.button2.Name = "button2";
        	this.button2.Size = new System.Drawing.Size(88, 23);
        	this.button2.TabIndex = 1;
        	this.button2.Text = "Eraser";
        	this.button2.UseVisualStyleBackColor = true;
        	this.button2.Click += new System.EventHandler(this.eraser_click);
        	// 
        	// button1
        	// 
        	this.button1.Location = new System.Drawing.Point(6, 19);
        	this.button1.Name = "button1";
        	this.button1.Size = new System.Drawing.Size(88, 23);
        	this.button1.TabIndex = 0;
        	this.button1.Text = "Brush";
        	this.button1.UseVisualStyleBackColor = true;
        	this.button1.Click += new System.EventHandler(this.brush_click);
        	// 
        	// flowLayoutPanel1
        	// 
        	this.flowLayoutPanel1.BackColor = System.Drawing.SystemColors.Control;
        	this.flowLayoutPanel1.Controls.Add(this.groupBox1);
        	this.flowLayoutPanel1.Controls.Add(this.groupBox2);
        	this.flowLayoutPanel1.Location = new System.Drawing.Point(761, 0);
        	this.flowLayoutPanel1.Name = "flowLayoutPanel1";
        	this.flowLayoutPanel1.Size = new System.Drawing.Size(150, 668);
        	this.flowLayoutPanel1.TabIndex = 6;
        	// 
        	// groupBox2
        	// 
        	this.groupBox2.Controls.Add(this.button8);
        	this.groupBox2.Controls.Add(this.button4);
        	this.groupBox2.Controls.Add(this.button5);
        	this.groupBox2.Controls.Add(this.button6);
        	this.groupBox2.Location = new System.Drawing.Point(3, 294);
        	this.groupBox2.Name = "groupBox2";
        	this.groupBox2.Size = new System.Drawing.Size(144, 141);
        	this.groupBox2.TabIndex = 5;
        	this.groupBox2.TabStop = false;
        	this.groupBox2.Text = "File";
        	// 
        	// button8
        	// 
        	this.button8.Location = new System.Drawing.Point(6, 106);
        	this.button8.Name = "button8";
        	this.button8.Size = new System.Drawing.Size(88, 23);
        	this.button8.TabIndex = 3;
        	this.button8.Text = "Save(CTRL+S)";
        	this.button8.UseVisualStyleBackColor = true;
        	this.button8.Click += new System.EventHandler(this.save_click);
        	// 
        	// button4
        	// 
        	this.button4.Location = new System.Drawing.Point(6, 77);
        	this.button4.Name = "button4";
        	this.button4.Size = new System.Drawing.Size(88, 23);
        	this.button4.TabIndex = 2;
        	this.button4.Text = "Select folder";
        	this.button4.UseVisualStyleBackColor = true;
        	this.button4.Click += new System.EventHandler(this.sel_folder_click);
        	// 
        	// button5
        	// 
        	this.button5.Location = new System.Drawing.Point(6, 48);
        	this.button5.Name = "button5";
        	this.button5.Size = new System.Drawing.Size(88, 23);
        	this.button5.TabIndex = 1;
        	this.button5.Text = "Previous(M)";
        	this.button5.UseVisualStyleBackColor = true;
        	this.button5.Click += new System.EventHandler(this.prev_click);
        	// 
        	// button6
        	// 
        	this.button6.Location = new System.Drawing.Point(6, 19);
        	this.button6.Name = "button6";
        	this.button6.Size = new System.Drawing.Size(88, 23);
        	this.button6.TabIndex = 0;
        	this.button6.Text = "Next(N)";
        	this.button6.UseVisualStyleBackColor = true;
        	this.button6.Click += new System.EventHandler(this.next_click);
        	// 
        	// Form1
        	// 
        	this.AccessibleRole = System.Windows.Forms.AccessibleRole.TitleBar;
        	this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
        	this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
        	this.BackColor = System.Drawing.SystemColors.ControlLightLight;
        	this.ClientSize = new System.Drawing.Size(913, 667);
        	this.Controls.Add(this.flowLayoutPanel1);
        	this.DoubleBuffered = true;
        	this.KeyPreview = true;
        	this.MaximumSize = new System.Drawing.Size(1800, 1600);
        	this.Name = "Form1";
        	this.Text = "Training creator";
        	this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.KeyDown_handl);
        	this.groupBox1.ResumeLayout(false);
        	this.groupBox1.PerformLayout();
        	((System.ComponentModel.ISupportInitialize)(this.trackBar1)).EndInit();
        	this.flowLayoutPanel1.ResumeLayout(false);
        	this.groupBox2.ResumeLayout(false);
        	this.ResumeLayout(false);
        }
        private System.Windows.Forms.Button button9;
        private System.Windows.Forms.Button button6;
        private System.Windows.Forms.Button button5;
        private System.Windows.Forms.Button button4;
        private System.Windows.Forms.Button button8;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.Button button3;
        private System.Windows.Forms.TrackBar trackBar1;
        private System.Windows.Forms.Button button7;
        
        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.FlowLayoutPanel flowLayoutPanel1;
        
        void Label2Click(object sender, System.EventArgs e)
        {
        	
        }
    }
}

