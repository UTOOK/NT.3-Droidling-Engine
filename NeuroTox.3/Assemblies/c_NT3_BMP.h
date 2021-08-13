//WARNING! DOES NOT USE THE STANDARD ASSEMBLY FUNCTIONS!

//Images are fed as input
//These are then built into the memory at a given level.
//Or they are evaluated on a certain level using a set of parameters.
//The eval will be recursive in that it will take many passes to complete.
//The output can be swapped for the input to continue with the next step of the eval.
class c_NT3_BMP
{
private:
     
     //This will hold the output from the networks until done, then this will be applied to the output BMP.
     c_Raw_Table_Cell_2D Output_Cell[3];
     
     //The current XY
     COORD Current_Position;
     
     //The current block size for reading in the eval data.
     int Current_Size;
     
     //The MSC handles the construct inputs.
     c_NT3_MSC_NonStrict MSC;
     c_NT3_Construct_2D Construct[3];
     
     c_NT3_MSC Top_Struct;
     
     //The current block of data to submit to the network.
     //One block for each RGB
     c_Raw_Table_Cell_2D Current_Block_Data[3];
     
     //The current color being evaluated. 0 = R, 1 = G, 2 = B
     int Current_Color;
     
     //The current dimensions.
     int Current_Width;
     int Current_Height;
     
     //Whether or not it is the first run through.
     bool flg_First_Run;
     
public:
     
     c_Image_Handler Input;
     c_Image_Handler Output;
     
     
     //The maximun height to build at, and to eval at.
     int MAX_Tier;
     
     //The minimun height to eval at.
     int MIN_Tier;
     
     //The maximum amount that the output is allowed to change an image.
     //For example with a diffusion modifier of 100 the amount a pixel can change is (+-)100.
     //So a pixel with an R value of 150 can at most change to either 50 or 250.
     double Diffusion_Modifier;
     
     //Whether or not to do a grayscale vs color.
     //0: Grayscale
     //1: RGB
     //2: R
     //3: G
     //4: B
     int settings_Color_Mode;
     
     c_NT3_BMP()
     {
          MAX_Tier = -1;
          MIN_Tier = 0;
          Diffusion_Modifier = 50;
          
          Current_Position.X = 0;
          Current_Position.Y = 0;
          Current_Size = 0;
          
          Current_Width = 0;
          Current_Height = 0;
          
          settings_Color_Mode = 0;
          
          flg_First_Run = true;
          
          update_Construct_Settings();
     }
     
     ~c_NT3_BMP()
     {
          
     }
     
     void set_MAX_Tier(int p_MAX_Tier){ MAX_Tier = p_MAX_Tier; update_Construct_Settings(); }
     void set_MIN_Tier(int p_MIN_Tier){ MIN_Tier = p_MIN_Tier; }
     void set_Diffusion_Modifier(double p_Diffusion_Modifier){ Diffusion_Modifier = p_Diffusion_Modifier; }
     
     void update_Construct_Settings()
     {
          Construct[0].set_MAX_Tier(MAX_Tier);
          Construct[1].set_MAX_Tier(MAX_Tier);
          Construct[2].set_MAX_Tier(MAX_Tier);
     }
     
     //Submits an input image. Accepts HBITMAP
     //Takes the given image and reads it into the input, after that it splits the input into the RGB.
     void Submit_Input(c_Image_Handler * p_IMG, int p_Output_Option = 0)
     {
          Input.set_HBITMAP(p_IMG);
          Input.grayscale_Bitmap();
          if (settings_Color_Mode == 1){ Input.separate_Bitmap(); }
          
          //Do not split the output, no need.
          Output.set_HBITMAP(p_IMG);
          
          Current_Width = p_IMG->BMP.bmWidth;
          Current_Height = p_IMG->BMP.bmHeight;
          
          if (p_Output_Option == 0)
          {
               Output_Cell[0].set_IMG_Cell_Data(&(Input.RGB_Input_Cell[0]));
               if (settings_Color_Mode == 1){ Output_Cell[1].set_IMG_Cell_Data(&(Input.RGB_Input_Cell[1])); }
               if (settings_Color_Mode == 1){ Output_Cell[2].set_IMG_Cell_Data(&(Input.RGB_Input_Cell[2])); }
          }
          else if (p_Output_Option == 1)
          {
               Output_Cell[0].resize_With_Reset(Current_Width, Current_Height);
               if (settings_Color_Mode == 1){ Output_Cell[1].resize_With_Reset(Current_Width, Current_Height); }
               if (settings_Color_Mode == 1){ Output_Cell[2].resize_With_Reset(Current_Width, Current_Height); }
          }
          
          flg_First_Run = true;
     }
     
     //Takes the Input.RGB_Input_Cell and submits it to the network for building.
     void Build_RGB()
     {
          cout << "\n\n Build_RGB()";
          
          cout << "\n R:";
          Construct[0].in_IMG_Cell(0, 0, &(Input.RGB_Input_Cell[0]));
          Construct[0].Build();
          //Construct.output_CAN_State();
          
          cout << "\n G:";
          Construct[1].in_IMG_Cell(0, 0, &(Input.RGB_Input_Cell[1]));
          Construct[1].Build();
          //Construct.output_CAN_State();
          
          cout << "\n B:";
          Construct[2].in_IMG_Cell(0, 0, &(Input.RGB_Input_Cell[2]));
          Construct[2].Build();
          //Construct.output_CAN_State();
          cout << "\n Done...";
     }
     
     //Takes the Input.RGB_Input_Cell and submits it to the network for building.
     void Build_Grayscale()
     {
          cout << "\n\n Build_Grayscale()";
          
          cout << "\n Gr:";
          Construct[0].in_IMG_Cell(0, 0, &(Input.RGB_Input_Cell[0]));
          Construct[0].Build();
          //Construct.output_CAN_State();
          
          cout << "\n Done...";
     }
     
     //Gathers the current block from the input RGB_Input_Cell.
     void gather_Current_Block()
     {
          //**DBGFH "\n gather_Current_Block()";
          //set_IMG_Cell_Data_Block(c_Raw_Table_Cell_2D * p_Cell, int p_X, int p_Y, int p_Size)
          if (flg_First_Run)
          {
               Current_Block_Data[Current_Color].set_IMG_Cell_Data_Block(&(Input.RGB_Input_Cell[Current_Color]), Current_Position.X, Current_Position.Y, Current_Size);
          }
          else
          {
               Current_Block_Data[Current_Color].set_IMG_Cell_Data_Block(&(Output_Cell[Current_Color]), Current_Position.X, Current_Position.Y, Current_Size);
          }
          //cout << "\n\n ("; oint(0, 8, Current_Position.X); cout << ", "; oint(0, 7, Current_Position.Y); cout << ")";
          //cout << "\n"; Current_Block_Data[0].output_C();
          //cout << "\n"; Current_Block_Data[1].output_C();
          //cout << "\n"; Current_Block_Data[2].output_C();
     }
     
     //Submits the current block to the construct.
     void submit_Current_Block_Data()
     {
          //**DBGFH "\n submit_Current_Block_Data()";
          Construct[Current_Color].in_IMG_Cell(0, 0, &(Current_Block_Data[Current_Color]));
     }
     
     //Evaluate and gather the results.
     void eval_Current_Block()
     {
          //**DBGFH "\n eval_Current_Block()";
          Construct[Current_Color].Eval();
     }
     
     //Gathers the current output into the output cell.
     //For now we will be gathering all of the outputs into an averaged output and averaging that with the output image.
     void gather_Current_Output()
     {
          //**DBGFH "\n gather_Current_Output()";
          
          int tmp_CurRGB = 0;
          int tmp_CurOut = 0;
          int tmp_Output = 0;
          
          int tmp_Index = 0;
          int tmp_Highest_Charge = 0;
          //double tmp_Charge_Modifier = 0.0f;
          
          
          //Average the output with the values of the BMP.                    
          //If no patterns were found move on.
          
          if (Construct[Current_Color].tbl_Pattern_Output_C.Number_Of_Rows == 0){ /*cout << "\n NO PATTERNS";*/ return; }
          
          string tmp_Title = "T(" + int2str(Current_Width) + ", " + int2str(Current_Height) + ") Cur(" + int2str(Current_Position.X) + ", " + int2str(Current_Position.Y) + ") COL:" + int2str(Current_Color) + " #P:" + int2str(Construct[Current_Color].tbl_Pattern_Output_C.Rows[0]->Depth);
          set_Console_Title(tmp_Title);
          
          //cout << " " << Construct[Current_Color].tbl_Pattern_Output_C.Rows[0]->Depth;
          //Construct[Current_Color].tbl_Pattern_Output_C.output_C();
          
          //ostr(0, 7, "\n   Current_Size->"); oint(0, 13, Current_Size);
          
          tmp_Highest_Charge = 0;
          tmp_Index = 0;
          
          
          //Loop through the outputs and find the highest charged one.
          for (int cou_P=0;cou_P<Construct[Current_Color].tbl_Pattern_Output_C.Rows[0]->Depth;cou_P++)
          {
               if (Construct[Current_Color].tbl_Charge_Output_C.get_I(0, cou_P, 0) > tmp_Highest_Charge)
               {
                    tmp_Highest_Charge = Construct[Current_Color].tbl_Charge_Output_C.get_I(0, cou_P, 0);
                    tmp_Index = cou_P;
               }
               if (Construct[Current_Color].tbl_Charge_Output_C.get_I(0, cou_P, 0) == tmp_Highest_Charge)
               {
                    if (!(rand() % 2))
                    {
                         tmp_Index = cou_P;
                    }
               }
          }
          //Construct[Current_Color].tbl_Charge_Output_C.output_I();
          
          //Loop through all of the output images in the network gathering the patterns into the output cell.
          //for (int cou_P=0;cou_P<Construct[Current_Color].tbl_Pattern_Output_C.Rows[0]->Depth;cou_P++)
          {
               for (int cou_X=0;cou_X<(Current_Size);cou_X++)
               {
                    for (int cou_Y=0;cou_Y<(Current_Size);cou_Y++)
                    {
                         //Get the output value.
                         //tmp_CurRGB = Construct[Current_Color].tbl_Pattern_Output_C.get_I(0, cou_P, cou_X, cou_Y);
                         tmp_CurRGB = Construct[Current_Color].tbl_Pattern_Output_C.get_I(0, tmp_Index, cou_X, cou_Y);
                         
                         //Get the current output value. This SHOULD exist.
                         tmp_CurOut = (Output_Cell[Current_Color].Data[Current_Position.X + cou_X] [Current_Position.Y + cou_Y]).I;
                         
                         //Get the percentage of the highest charge that this current pattern is.
                         //tmp_Charge_Modifier = double (double (Construct[Current_Color].tbl_Charge_Output_C.get_I(0, cou_P, 0)) / double (tmp_Highest_Charge));
                         
                         //Average them
                         tmp_Output = (tmp_CurRGB + tmp_CurOut) / 2;
                         //tmp_Output = (tmp_CurRGB + tmp_CurOut) / 2;
                         //tmp_Output = (int (double (tmp_CurOut - tmp_CurRGB) * tmp_Charge_Modifier)) + tmp_CurRGB;
                         
                         //Submit the new value back into the output image.
                         Output_Cell[Current_Color].Data[Current_Position.X + cou_X] [Current_Position.Y + cou_Y].I = tmp_Output;
                         
                         //cout << "\n (" << cou_X << ", " << cou_Y << ") CMod->" << tmp_Charge_Modifier << " CurRGB->" << tmp_CurRGB << " CurOut->" << tmp_CurOut << " Output->" << tmp_Output << " HCharge->" << tmp_Highest_Charge << " CCharge->" << Construct[Current_Color].tbl_Charge_Output_C.get_I(0, cou_P, 0) << " CDepth->" << Construct[Current_Color].tbl_Pattern_Output_C.Rows[0]->Depth;
                    }
               }
               //cout << "\n\n";
               //Construct[Current_Color].tbl_Pattern_Output_C.Rows[0]->Cells[0]->output_C();
          }
     }
     
     //Applies the gathered output to the output image.
     void gather_Final_Output()
     {
          //cout << "\n\n gather_Final_Output()->" << this;
          //ostr(0, 12, " Output_Cell_Size->("); oint(0, 8, Output_Cell[Current_Color].Width); ostr(0, 12, ", "); oint(0, 7, Output_Cell[Current_Color].Height);
          cout << "\n\n " << Diffusion_Modifier;
          if (settings_Color_Mode == 0){ Output.average_2D_Cell_Into_BMP_Source_Grayscale(Output_Cell, Diffusion_Modifier); }
          if (settings_Color_Mode == 1){ Output.average_2D_Cell_Into_BMP_Source_RGB(Output_Cell, Diffusion_Modifier); }
     }
     
     //Evals all tiers stepping by one.
     void eval_All_Tiers()
     {
          for (int cou_T=MAX_Tier;cou_T>=MIN_Tier;cou_T--)
          {
               cout << "\n Current_Tier->"; oint(0, 12, cou_T);
               //The current tier determines the size of the input to evaluate.
               //The input is a bloack of data (cou_T + 1) in height and width.
               Current_Size = (cou_T);
               for (int cou_Color=0;cou_Color<3;cou_Color++)
               {
                    Current_Color = cou_Color;
                    for (int cou_X=0;cou_X<(Current_Width - Current_Size);cou_X+=(Current_Size))
                    {
                         for (int cou_Y=0;cou_Y<(Current_Height - Current_Size);cou_Y+=(Current_Size))
                         {
                              Current_Position.X = cou_X;
                              Current_Position.Y = cou_Y;
                              
                              //cout << "\n\n\n (" << Current_Position.X << ", " << Current_Position.Y << ")  Current_Block_Size->" << Current_Size;
                              
                              //Gather the current block
                              gather_Current_Block();
                              
                              //Submit the block to the network for eval
                              submit_Current_Block_Data();
                              
                              //Eval the current block.
                              eval_Current_Block();
                              
                              //Gather the results
                              gather_Current_Output();
                         }
                    }
               }
          }
     }
     
     //Evals just the top and low tiers.
     void eval_High_Low()
     {
          int tmp_Tier = MAX_Tier;
          int tmp_Step = 0;
          
          cout << "\n Current_Tier->"; oint(0, 12, tmp_Tier);
          //The current tier determines the size of the input to evaluate.
          //The input is a bloack of data (cou_T + 1) in height and width.
          
          Current_Size = (tmp_Tier);
          tmp_Step = (Current_Size / 3);
          for (int cou_Color=0;cou_Color<3;cou_Color++)
          {
               Current_Color = cou_Color;
               for (int cou_X=0;cou_X<(Current_Width - Current_Size);cou_X+=(tmp_Step))
               {
                    for (int cou_Y=0;cou_Y<(Current_Height - Current_Size);cou_Y+=(tmp_Step))
                    {
                         Current_Position.X = cou_X;
                         Current_Position.Y = cou_Y;
                         
                         //cout << "\n\n\n (" << Current_Position.X << ", " << Current_Position.Y << ")  Current_Block_Size->" << Current_Size;
                         
                         //Gather the current block
                         gather_Current_Block();
                         
                         //Submit the block to the network for eval
                         submit_Current_Block_Data();
                         
                         //Eval the current block.
                         eval_Current_Block();
                         
                         //Gather the results
                         gather_Current_Output();
                    }
               }
          }
          
          tmp_Tier = MIN_Tier;
          //cout << "\n Current_Tier->"; oint(0, 12, tmp_Tier);
          //The current tier determines the size of the input to evaluate.
          //The input is a bloack of data (cou_T + 1) in height and width.
          
          Current_Size = (tmp_Tier);
          tmp_Step = (Current_Size / 3);
          for (int cou_Color=0;cou_Color<3;cou_Color++)
          {
               Current_Color = cou_Color;
               for (int cou_X=0;cou_X<(Current_Width - Current_Size);cou_X+=(tmp_Step))
               {
                    for (int cou_Y=0;cou_Y<(Current_Height - Current_Size);cou_Y+=(tmp_Step))
                    {
                         Current_Position.X = cou_X;
                         Current_Position.Y = cou_Y;
                         
                         //cout << "\n\n\n (" << Current_Position.X << ", " << Current_Position.Y << ")  Current_Block_Size->" << Current_Size;
                         
                         //Gather the current block
                         gather_Current_Block();
                         
                         //Submit the block to the network for eval
                         submit_Current_Block_Data();
                         
                         //Eval the current block.
                         eval_Current_Block();
                         
                         //Gather the results
                         gather_Current_Output();
                    }
               }
          }
     }
     
     
     //Evals just the top and low tiers.
     void eval_High()
     {
          int tmp_Tier = MAX_Tier;
          int tmp_Step = 0;
          //cout << "\n Current_Tier->"; oint(0, 12, tmp_Tier);
          //The current tier determines the size of the input to evaluate.
          //The input is a block of data (cou_T + 1) in height and width.
          
          Current_Size = (tmp_Tier);
          tmp_Step = (1);
          for (int cou_Color=0;cou_Color<3;cou_Color++)
          {
               Current_Color = cou_Color;
               for (int cou_X=0;cou_X<(Current_Width - Current_Size);cou_X+=(tmp_Step))
               {
                    for (int cou_Y=0;cou_Y<(Current_Height - Current_Size);cou_Y+=(tmp_Step))
                    {
                         Current_Position.X = cou_X;
                         Current_Position.Y = cou_Y;
                         
                         cout << "\n\n\n (" << Current_Position.X << ", " << Current_Position.Y << ")  Current_Block_Size->" << Current_Size;
                         
                         //Gather the current block
                         gather_Current_Block();
                         
                         //Submit the block to the network for eval
                         submit_Current_Block_Data();
                         
                         //Eval the current block.
                         eval_Current_Block();
                         
                         //Gather the results
                         gather_Current_Output();
                    }
               }
          }
     }
     
     //Eval RGB
     void Eval()
     {
          ostr(0, 13, "\n  Eval()");
          
          cout << "\n Current_Width->" << Current_Width << " Current_Height->" << Current_Height;
          
          eval_High();
          
          //ostr(0, 12, "\n\n ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~R");
          //Output_Cell[0].output_C();
          //ostr(0, 10, "\n\n ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~G");
          //Output_Cell[1].output_C();
          //ostr(0, 9, "\n\n ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~B");
          //Output_Cell[2].output_C();
          
          gather_Final_Output();
     }
     
     //Evals a block of data.
     void eval_Block(int p_X, int p_Y, int p_Size)
     {
          //**DBGFH "\n  eval_Block(" << p_X << ", " << p_Y << ") SIZE->" << p_Size;
          int tmp_Step = 1;
          
          //for (int cou_T=MAX_Tier;cou_T>=MIN_Tier;cou_T--)
          //for (int cou_T=MIN_Tier;cou_T<MAX_Tier;cou_T++)
          int cou_T = MAX_Tier;
          {
               cout << "\n Current_Tier->"; oint(0, 12, cou_T);
               //The current tier determines the size of the input to evaluate.
               //The input is a bloack of data (cou_T + 1) in height and width.
               Current_Size = (cou_T);
               tmp_Step = (Current_Size);
               if (tmp_Step < 1){ tmp_Step = 1; }
               for (int cou_Color=0;cou_Color<3;cou_Color++)
               {
                    Current_Color = cou_Color;
                    for (int cou_X=p_X;cou_X<(p_Size + p_X);cou_X+=tmp_Step)
                    {
                         if (cou_X >= (Current_Width - Current_Size)){ continue; }
                         for (int cou_Y=p_Y;cou_Y<(p_Size + p_Y);cou_Y+=tmp_Step)
                         {
                              if (cou_Y >= (Current_Height - Current_Size)){ continue; }
                              
                              Current_Position.X = cou_X;
                              Current_Position.Y = cou_Y;
                              
                              //cout << "\n (" << Current_Position.X << ", " << Current_Position.Y << ")  Current_Block_Size->" << Current_Size;
                              
                              //Gather the current block
                              gather_Current_Block();
                              
                              //Submit the block to the network for eval
                              submit_Current_Block_Data();
                              
                              //Eval the current block.
                              eval_Current_Block();
                              
                              //Gather the results
                              gather_Current_Output();
                         }
                    }
               }
          }
     }
     
     //Evals a given block from the highest tier down to the lowest.
     void Eval(int p_X, int p_Y, int p_Size)
     {
          ostr(0, 13, "\n Eval_Block "); oint(0, 7, p_X); cout << " "; oint(0, 7, p_Y);
          
          eval_Block(p_X, p_Y, p_Size);
          
          gather_Final_Output();
     }
};
