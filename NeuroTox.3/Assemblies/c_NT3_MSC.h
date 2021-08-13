//This class is the MSC for working with multiple inputs.
class c_NT3_MSC
{
private:
     
     
     //The Discharging_Buffers. One for each input.
     c_NT3_Data_Buffer * Discharging_Buffers;
     
public:
     
     //The constructs in the assembly.
     c_NT3_Construct_1D ** Constructs;
     
     //The current number of constructs.
     int Construct_Count;
     
     //This construct is the MSC that takes the treetops from the raw constructs as input.
     c_NT3_Construct_1D MSC;
     
     //The name of the construct.
     string Name;
     
     //Whether or not a construct had outputs.
     int * Construct_Has_Output;
     
     int * Vision_Output_Count;
     long long int * Vision_Charge;
     
     int Construct_Vision_Output_Count;
     
     //==--   TABLES
     
     //Input table.
     c_Raw_Table_1D tbl_Input;
     
     //The goal input table. Used for autocalibrate
     c_Raw_Table_1D tbl_Input_Goal;
     
     //Output tables.
     c_Raw_Table_1D tbl_Assembly_Treetops;
     
     //Outputs the visions.
     c_Raw_Table_1D tbl_Vision_Output;
     
     //The charges for the vision.
     c_Raw_Table_1D tbl_Vision_Output_Charge;
     
     //==--   SETTINGS
     
     //Whether or not to gather the outputs into charging buffers before the MSC is discharged.
     int settings_Compress_MSC_Outputs;
     
     //The APT for the discharging of the treetops.
     double Action_Potential_Threshold;
     
     //The base charge for the discharging of the treetops.
     int Base_Charge;
     
     //Settings for the autocalibration feature.
     double settings_APT_Inc;
     double settings_APT_LInc;
     
     double settings_MC_Inc;
     double settings_MC_LInc;
     
     long long int settings_BC_Inc;
     long long int settings_BC_LInc;
     
     double settings_FT_Inc;
     double settings_FT_LInc;
     
     int settings_Auto_Row_Limit;
     
     int settings_Accuracy_Upper;
     
     int settings_Vision_Fog;
     
     int settings_Vision_Verify;
     c_Raw_Table_1D tbl_Verified_Output;
     
     //This controls how many outputs are read into the vision.
     int settings_Vision_Cap;
     
     c_NT3_MSC()
     {
          Name = "Generic_Assembly";
          tbl_Input.Name = "Assembly_Input";
          tbl_Input_Goal.Name = "Assembly_Input_Goal";
          
          tbl_Vision_Output.Name = "Assembly_Vision";
          tbl_Vision_Output_Charge.Name = "Assembly_Vision_Charge";
          tbl_Verified_Output.Name = "Assembly_Vision_Verified_Output";
          
          
          Constructs = NULL;
          Construct_Has_Output = NULL;
          Vision_Output_Count = NULL;
          Vision_Charge = NULL;
          
          Construct_Count = 0;
          MSC.set_Name("MSC");
          MSC.settings_Action_Potential_Threshold = 0;
          MSC.settings_Modifier_Charge = 1;
          Discharging_Buffers = NULL;
          
          
          settings_APT_Inc = .05;
          settings_APT_LInc = .01;
          
          settings_MC_Inc = .05;
          settings_MC_LInc = .01;
          
          settings_BC_Inc = 2500;
          settings_BC_LInc = 100;
          
          settings_FT_Inc = .1;
          settings_FT_LInc = .01;
          
          settings_Compress_MSC_Outputs = 0;
          
          settings_Auto_Row_Limit = 0;
          
          settings_Accuracy_Upper = -1;
          
          settings_Vision_Fog = 10;
          
          Action_Potential_Threshold = 0.25f;
          Base_Charge = 10000;
          
          
          Construct_Vision_Output_Count = 0;
     }
     
     ~c_NT3_MSC()
     {
          cout << "\n ~c_NT3_MSC " << Name << " " << this << ".........."; cout.flush();
          
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               delete Constructs[cou_Index];
               Constructs[cou_Index] = NULL;
          }
          delete [] Constructs;
          Constructs = NULL;
          delete [] Construct_Has_Output;
          Construct_Has_Output = NULL;
          Vision_Output_Count = NULL;
          delete [] Vision_Output_Count;
          Vision_Charge = NULL;
          delete [] Vision_Charge;
          
          delete [] Discharging_Buffers;
          Discharging_Buffers = NULL;
          
          
          
          cout << "\n ~~~c_NT3_MSC " << Name << " " << this << ".........."; cout.flush();
     }
     
     //Sets the name of the assembly.
     void set_Name(string p_Name)
     {
          Name = p_Name;
          tbl_Input.Name = Name + "__Input";
          tbl_Input_Goal.Name = Name + "__Input_Goal";
          
          string tmp_Construct_Name = "";
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               tmp_Construct_Name = Name + "___Input_" + int2str(cou_Index);
               Constructs[cou_Index]->set_Name(tmp_Construct_Name);
          }
     }
     
     ////==-----------------------+
     //==--   EVAL FUNCTIONS
     ////==-----------------------+
     
     //Eval with no RC.
     void Eval()
     { 
          Build_Query(); 
          
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               Constructs[cou_Index]->Eval();
               Construct_Has_Output[cou_Index] = 0;
               
               Vision_Output_Count[cou_Index] = 0;
               Vision_Charge[cou_Index] = 0;
               //If either table has rows then that means the construct had outputs.
               if (Constructs[cou_Index]->tbl_Treetops_Output_L.Number_Of_Rows > 0){ Construct_Has_Output[cou_Index] = 1; }
               if (Constructs[cou_Index]->tbl_Treetops_Output_R.Number_Of_Rows > 0){ Construct_Has_Output[cou_Index] = 1; }
          }
          
          //*cout << "\n\n\n\n\n\n MSC Eval";
          
          MSC.reset_Inputs();
          
          gather_MSC_Eval_Input_L();
          gather_MSC_Eval_Input_R();
          
          MSC.Eval_Aux_Inputs();
          //*MSC.output_Input_Tables();
          //*MSC.output_Output_Tables();
          
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               Constructs[cou_Index]->reset_Outputs();
          }
          
          tbl_Verified_Output.reset();
          if (settings_Compress_MSC_Outputs == 0)
          {
               discharge_MSC_Treetops_No_Compress_L();
               discharge_MSC_Treetops_No_Compress_R();
          }
          
          if (settings_Compress_MSC_Outputs == 1)
          {
               discharge_MSC_Treetops_L();
               discharge_MSC_Treetops_R();
          }
          
          gather_Vision_Verified_Table();
          
          //output_Output_Tables();
     } 
     
     //Eval with RC.
     void Eval_RC()
     { 
          Build_Query(); 
          
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               Constructs[cou_Index]->Eval_RC();
               Construct_Has_Output[cou_Index] = 0;
               Vision_Output_Count[cou_Index] = 0;
               Vision_Charge[cou_Index] = 0;
               //If either table has rows then that means the construct had outputs.
               if (Constructs[cou_Index]->tbl_Treetops_Output_L.Number_Of_Rows > 0){ Construct_Has_Output[cou_Index] = 1; }
               if (Constructs[cou_Index]->tbl_Treetops_Output_R.Number_Of_Rows > 0){ Construct_Has_Output[cou_Index] = 1; }
          }
          
          MSC.reset_Inputs();
          
          gather_MSC_Eval_Input_L();
          gather_MSC_Eval_Input_R();
          
          MSC.Eval_Aux_Inputs_RC();
          //MSC.output_Input_Tables();
          //MSC.output_Output_Tables();
          
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               Constructs[cou_Index]->reset_Outputs();
          }
          
          tbl_Verified_Output.reset();
          
          if (settings_Compress_MSC_Outputs == 0)
          {
               discharge_MSC_Treetops_No_Compress_L();
               discharge_MSC_Treetops_No_Compress_R();
          }
          
          if (settings_Compress_MSC_Outputs == 1)
          {
               discharge_MSC_Treetops_L();
               discharge_MSC_Treetops_R();
          }
          
          gather_Vision_Verified_Table();
          //output_Output_Tables();
     }
     
     //Eval with RC.
     void Eval_Freely()
     { 
          Build_Query(); 
          
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               if (Constructs[cou_Index]->sauto_Use_RC)
               {
                    Constructs[cou_Index]->Eval_RC();
               }
               else
               {
                    Constructs[cou_Index]->Eval();
               }
               
               Construct_Has_Output[cou_Index] = 0;
               Vision_Output_Count[cou_Index] = 0;
               Vision_Charge[cou_Index] = 0;
               //If either table has rows then that means the construct had outputs.
               if (Constructs[cou_Index]->tbl_Treetops_Output_L.Number_Of_Rows > 0){ Construct_Has_Output[cou_Index] = 1; }
               if (Constructs[cou_Index]->tbl_Treetops_Output_R.Number_Of_Rows > 0){ Construct_Has_Output[cou_Index] = 1; }
          }
          /*
          MSC.reset_Inputs();
          
          gather_MSC_Eval_Input_L();
          gather_MSC_Eval_Input_R();
          
          if (MSC.sauto_Use_RC)
          {
               MSC.Eval_Aux_Inputs_RC();
          }
          else
          {
               MSC.Eval_Aux_Inputs();
          }
          //MSC.output_Input_Tables();
          //MSC.output_Output_Tables();
          
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               Constructs[cou_Index]->reset_Outputs();
          }
          
          tbl_Verified_Output.reset();
          
          if (settings_Compress_MSC_Outputs == 0)
          {
               discharge_MSC_Treetops_No_Compress_L();
               discharge_MSC_Treetops_No_Compress_R();
          }
          
          if (settings_Compress_MSC_Outputs == 1)
          {
               discharge_MSC_Treetops_L();
               discharge_MSC_Treetops_R();
          }
          
          gather_Vision_Verified_Table();
          //output_Output_Tables();
           * */
     }
     
     //Eval with RC.
     void Eval_Freely_Poly()
     { 
          Build_Query();
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               if (Constructs[cou_Index]->sauto_Use_RC)
               {
                    Constructs[cou_Index]->Eval_RC();
               }
               else
               {
                    Constructs[cou_Index]->Eval();
               }
               Construct_Has_Output[cou_Index] = 0;
               Vision_Output_Count[cou_Index] = 0;
               Vision_Charge[cou_Index] = 0;
               //If either table has rows then that means the construct had outputs.
               if (Constructs[cou_Index]->tbl_Treetops_Output_L.Number_Of_Rows > 0){ Construct_Has_Output[cou_Index] = 1; }
               if (Constructs[cou_Index]->tbl_Treetops_Output_R.Number_Of_Rows > 0){ Construct_Has_Output[cou_Index] = 1; }
          }
          MSC.reset_Inputs();
          
          gather_MSC_Eval_Input_L();
          gather_MSC_Eval_Input_R();
          
          //Make use of the full charging algo.
          if (MSC.sauto_Use_RC)
          {
               MSC.Eval_Aux_Poly_Inputs_RC();
               //MSC.Eval_Aux_Inputs_RC();
          }
          else
          {
               MSC.Eval_Aux_Poly_Inputs();
               //MSC.Eval_Aux_Inputs();
          }
          //MSC.output_Input_Tables();
          //MSC.output_Output_Tables();
          //MSC.output_PCAN_State();
          
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               Constructs[cou_Index]->reset_Outputs();
          }
          
          //MSC.tbl_Pattern_Output_L.output_C();
          //MSC.tbl_Charge_Output_L.output_I();
          
          tbl_Verified_Output.reset();
          
          discharge_MSC_Treetops_No_Compress_L();
          discharge_MSC_Treetops_No_Compress_R();
          
          gather_Vision_Verified_Table();
          //output_Output_Tables();
     }
     
     ////==-----------------------+
     //==--   BUILDING FUNCTIONS
     ////==-----------------------+
     
     //Builds full CAN.
     void Build()
     { 
          //*cout << "\n gather_Inputs()";
          //*tbl_Input.output_C();
          gather_Inputs(); 
          
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               //*cout << "\n Constructs[" << cou_Index << "]->Build();";
               Constructs[cou_Index]->Build();
          }
          //*cout << "\n gather_MSC_Input()";
          gather_MSC_Input();
          
          //*cout << "\n MSC.Build();";
          MSC.Build();
     } 
     
     //Builds RC CAN.
     void Build_RC()
     { 
          gather_Inputs();
          
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               Constructs[cou_Index]->Build_RC();
          }
          gather_MSC_Input();
          MSC.Build_RC();
     } 
     
     //Builds for query.
     void Build_Query()
     { 
          gather_Inputs(); 
          
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               Constructs[cou_Index]->Build_Query();
          }
     } 
     
     
     
     //Gatheres the inputs into the raw constructs, creates constructs if needed.
     
     ////==--------------------------+
     //==--   DISCHARGING FUNCTIONS
     ////==--------------------------+
     
     //Discharges the MSC treetops back through the raw constructs by first setting up the charging buffers. 
     //Then is submits the nodes from the MSC outputs to the charging buffers.
     //After the nodes have all been read in the buffers in the constructs are gathered() then discharged().
     //It does this by looping through the MSC output tables and extracting the NID|CID, charge, and position.
     //These are then submitted to the appropriate constructs.
     void discharge_MSC_Treetops_L()
     {
          u_Data_3 tmp_NIDCID;
          tmp_NIDCID.U = 0;
          u_Data_3 tmp_NID;
          tmp_NID.U = 0;
          u_Data_3 tmp_CID;
          tmp_CID.U = 0;
          u_Data_3 tmp_Charge;
          tmp_Charge.U = 0;
          int tmp_Position = 0;
          
          int tmp_Number_Of_Buffers = 0;
          
          //If the table is empty then return.
          if (MSC.tbl_Pattern_Output_L.Number_Of_Rows == 0){ return; }
          
          //Each row in the MSC output is read into the row in the Construct output. Row for Row.
          for (int cou_R=0;cou_R<MSC.tbl_Pattern_Output_L.Number_Of_Rows;cou_R++)
          {
               //Gather the number of buffers to hold the discharged nodes.
               tmp_Number_Of_Buffers = 0;
               
               for (int cou_C=0;cou_C<MSC.tbl_Pattern_Output_L.Rows[cou_R]->Depth;cou_C++)
               {
                    tmp_Position = MSC.tbl_Position_Output_L.get_I(cou_R, cou_C, 0);
                    if (tmp_Position > tmp_Number_Of_Buffers){ tmp_Number_Of_Buffers = tmp_Position; }
               }
               tmp_Number_Of_Buffers++;
               
               //Expand the buffers to accommodate the outputs.
               init_Discharging_Buffers(tmp_Number_Of_Buffers);
               
               for (int cou_C=0;cou_C<MSC.tbl_Pattern_Output_L.Rows[cou_R]->Depth;cou_C++)
               {
                    
                    tmp_Charge = MSC.tbl_Charge_Output_L.get(cou_R, cou_C, 0);
                    tmp_Position = MSC.tbl_Position_Output_L.get_I(cou_R, cou_C, 0);
                         
                    //Each data bit in the cell is one pattern to return.
                    for (int cou_D=0;cou_D<MSC.tbl_Pattern_Output_L.Rows[cou_R]->Cells[cou_C]->Length;cou_D++)
                    {
                         tmp_NIDCID = MSC.tbl_Pattern_Output_L.get(cou_R, cou_C, cou_D);
                         Discharging_Buffers[tmp_Position].submit(tmp_NIDCID, tmp_Charge.U);
                    }
               }
               
               c_NT3_Linked_Data_List * tmp_LL = NULL;
               
               for (int cou_P=0;cou_P<tmp_Number_Of_Buffers;cou_P++)
               {
                    tmp_LL = Discharging_Buffers[cou_P].Data_Buffer.Root;
                    while(tmp_LL != NULL)
                    {
                         tmp_Charge = tmp_LL->Charge;
                         tmp_NIDCID = tmp_LL->NID;
                         tmp_NID.U = tmp_NIDCID.U * .01;
                         tmp_CID.U = tmp_NIDCID.U - (tmp_NID.U * 100);
                         
                         Constructs[tmp_CID.U]->discharge_Treetop_By_NID_L(cou_P, tmp_NID, tmp_Charge, cou_R);
                         
                         tmp_LL = tmp_LL->Next;
                    }
               }
          }
     }
     
     void discharge_MSC_Treetops_R()
     {
          u_Data_3 tmp_NIDCID;
          tmp_NIDCID.U = 0;
          u_Data_3 tmp_NID;
          tmp_NID.U = 0;
          u_Data_3 tmp_CID;
          tmp_CID.U = 0;
          u_Data_3 tmp_Charge;
          tmp_Charge.U = 0;
          int tmp_Position = 0;
          
          int tmp_Number_Of_Buffers = 0;
          
          //If the table is empty then return.
          if (MSC.tbl_Pattern_Output_R.Number_Of_Rows == 0){ return; }
          
          //Each row in the MSC output is read into the row in the Construct output. Row for Row.
          for (int cou_R=0;cou_R<MSC.tbl_Pattern_Output_R.Number_Of_Rows;cou_R++)
          {
               //Gather the number of buffers to hold the discharged nodes.
               tmp_Number_Of_Buffers = 0;
               for (int cou_C=0;cou_C<MSC.tbl_Pattern_Output_R.Rows[cou_R]->Depth;cou_C++)
               {
                    tmp_Position = MSC.tbl_Position_Output_R.get_I(cou_R, cou_C, 0);
                    if (tmp_Position > tmp_Number_Of_Buffers){ tmp_Number_Of_Buffers = tmp_Position; }
               }
               tmp_Number_Of_Buffers++;
               
               //Expand the buffers to accommodate the outputs.
               init_Discharging_Buffers(tmp_Number_Of_Buffers);
               
               for (int cou_C=0;cou_C<MSC.tbl_Pattern_Output_R.Rows[cou_R]->Depth;cou_C++)
               {
                    
                    tmp_Charge = MSC.tbl_Charge_Output_R.get(cou_R, cou_C, 0);
                    tmp_Position = MSC.tbl_Position_Output_R.get_I(cou_R, cou_C, 0);
                         
                    //Each data bit in the cell is one pattern to return.
                    for (int cou_D=0;cou_D<MSC.tbl_Pattern_Output_R.Rows[cou_R]->Cells[cou_C]->Length;cou_D++)
                    {
                         tmp_NIDCID = MSC.tbl_Pattern_Output_R.get(cou_R, cou_C, cou_D);
                         Discharging_Buffers[tmp_Position].submit(tmp_NIDCID, tmp_Charge.U);
                    }
               }
               
               c_NT3_Linked_Data_List * tmp_LL = NULL;
               
               for (int cou_P=0;cou_P<tmp_Number_Of_Buffers;cou_P++)
               {
                    tmp_LL = Discharging_Buffers[cou_P].Data_Buffer.Root;
                    
                    while(tmp_LL != NULL)
                    {
                         tmp_Charge = tmp_LL->Charge;
                         tmp_NIDCID = tmp_LL->NID;
                         tmp_NID.U = tmp_NIDCID.U * .01;
                         tmp_CID.U = tmp_NIDCID.U - (tmp_NID.U * 100);
                         
                         Constructs[tmp_CID.U]->discharge_Treetop_By_NID_R(cou_P, tmp_NID, tmp_Charge, cou_R);
                         
                         tmp_LL = tmp_LL->Next;
                    }
               }
          }
     }
     
          
     //Sets the discharging buffers up for each output.
     void init_Discharging_Buffers(int p_Input_Depth)
     {
          delete [] Discharging_Buffers;
          Discharging_Buffers = NULL;
          Discharging_Buffers = new c_NT3_Data_Buffer[p_Input_Depth];
          for (int cou_Index=0;cou_Index<p_Input_Depth;cou_Index++)
          {
               Discharging_Buffers[cou_Index].Base_Charge = 10000;
               Discharging_Buffers[cou_Index].Modifier_Charge = 1;
               Discharging_Buffers[cou_Index].Action_Potential_Threshold = 0;
          }
     }
     
     
     void discharge_MSC_Treetops_No_Compress_L()
     {
          u_Data_3 tmp_NIDCID;
          tmp_NIDCID.U = 0;
          u_Data_3 tmp_NID;
          tmp_NID.U = 0;
          u_Data_3 tmp_CID;
          tmp_CID.U = 0;
          u_Data_3 tmp_Charge;
          tmp_Charge.U = 0;
          int tmp_Position = 0;
          
          int tmp_Cuttoff = Base_Charge * Action_Potential_Threshold;
          
          //If the table is empty then return.
          if (MSC.tbl_Pattern_Output_L.Number_Of_Rows == 0){ return; }
          
          //Each row in the MSC output is read into the row in the Construct output. Row for Row.
          for (int cou_R=0;cou_R<MSC.tbl_Pattern_Output_L.Number_Of_Rows;cou_R++)
          {
               //Discharge the outputs.
               for (int cou_C=0;cou_C<MSC.tbl_Pattern_Output_L.Rows[cou_R]->Depth;cou_C++)
               {
                    
                    tmp_Charge = MSC.tbl_Charge_Output_L.get(cou_R, cou_C, 0);
                    tmp_Position = MSC.tbl_Position_Output_L.get_I(cou_R, cou_C, 0);
                    
                    //cout << "\n if (" << tmp_Charge.I << " < " << tmp_Cuttoff << ")";
                    if (tmp_Charge.I < tmp_Cuttoff){ continue; }
                    
                    //Each data bit in the cell is one pattern to return.
                    for (int cou_D=0;cou_D<MSC.tbl_Pattern_Output_L.Rows[cou_R]->Cells[cou_C]->Length;cou_D++)
                    {
                         tmp_NIDCID = MSC.tbl_Pattern_Output_L.get(cou_R, cou_C, cou_D);
                         tmp_NID.U = tmp_NIDCID.U * .01;
                         tmp_CID.U = tmp_NIDCID.U - (tmp_NID.U * 100);
                         
                         Constructs[tmp_CID.U]->discharge_Treetop_By_NID_L(tmp_Position, tmp_NID, tmp_Charge, cou_R);
                    }
               }
          }
     }
     
     void discharge_MSC_Treetops_No_Compress_R()
     {
          u_Data_3 tmp_NIDCID;
          tmp_NIDCID.U = 0;
          u_Data_3 tmp_NID;
          tmp_NID.U = 0;
          u_Data_3 tmp_CID;
          tmp_CID.U = 0;
          u_Data_3 tmp_Charge;
          tmp_Charge.U = 0;
          int tmp_Position = 0;
          
          int tmp_Cuttoff = Base_Charge * Action_Potential_Threshold;
          
          //If the table is empty then return.
          if (MSC.tbl_Pattern_Output_R.Number_Of_Rows == 0){ return; }
          
          //Each row in the MSC output is read into the row in the Construct output. Row for Row.
          for (int cou_R=0;cou_R<MSC.tbl_Pattern_Output_R.Number_Of_Rows;cou_R++)
          {
               for (int cou_C=0;cou_C<MSC.tbl_Pattern_Output_R.Rows[cou_R]->Depth;cou_C++)
               {
                    
                    tmp_Charge = MSC.tbl_Charge_Output_R.get(cou_R, cou_C, 0);
                    tmp_Position = MSC.tbl_Position_Output_R.get_I(cou_R, cou_C, 0);
                         
                    if (tmp_Charge.I < tmp_Cuttoff){ continue; }
                    
                    //Each data bit in the cell is one pattern to return.
                    for (int cou_D=0;cou_D<MSC.tbl_Pattern_Output_R.Rows[cou_R]->Cells[cou_C]->Length;cou_D++)
                    {
                         tmp_NIDCID = MSC.tbl_Pattern_Output_R.get(cou_R, cou_C, cou_D);
                         tmp_NID.U = tmp_NIDCID.U * .01;
                         tmp_CID.U = tmp_NIDCID.U - (tmp_NID.U * 100);
                         
                         Constructs[tmp_CID.U]->discharge_Treetop_By_NID_R(tmp_Position, tmp_NID, tmp_Charge, cou_R);
                    }
               }
          }
     }
     
          
     void gather_Vision_Verified_Table()
     {
          u_Data_3 tmp_Charge;
          tmp_Charge.U = 0;
          int tmp_Track = 0;
          
          int tmp_Cuttoff = Base_Charge * Action_Potential_Threshold;
          
          //If the table is empty then return.
          if (MSC.tbl_Pattern_Output_L.Number_Of_Rows == 0){ return; }
          
          //Each row in the MSC output is read into the row in the Construct output. Row for Row.
          for (int cou_R=0;cou_R<MSC.tbl_Pattern_Output_L.Number_Of_Rows;cou_R++)
          {
               for (int cou_C=0;cou_C<MSC.tbl_Pattern_Output_L.Rows[cou_R]->Depth;cou_C++)
               {
                    tmp_Charge = MSC.tbl_Charge_Output_L.get(cou_R, cou_C, 0);
                    
                    if (tmp_Charge.I < tmp_Cuttoff){ continue; }
                    
                    tbl_Verified_Output.set_Int(cou_R, tmp_Track, 1);
                    tmp_Track++;
               }
          }
     }
     
     ////==-----------------------------------+
     //==--   INPUTS AND OUTPUT MANIPULATION
     ////==-----------------------------------+
     
     //If there are more inputs than constructs then allocate new constructs for those inputs.
     void gather_Inputs()
     {
          resize(tbl_Input.Number_Of_Rows);
     }
     
     //Gathers the treetops from the built constructs and inputs them into the MSC.
     void gather_MSC_Input()
     {
          MSC.reset_Inputs();
          
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               if (Constructs[cou_Index]->tbl_Treetops.Number_Of_Rows == 0){ continue; }
               
               for (int cou_Cell=0;cou_Cell<Constructs[cou_Index]->tbl_Treetops.Rows[0]->Depth;cou_Cell++)
               {
                    //if ((Constructs[cou_Index]->tbl_Treetops.Rows[0]->Cells[cou_Cell]->Data[0]).U == 0){ continue; }
                    MSC.tbl_Input.add_Data_Int(0, cou_Cell, (((Constructs[cou_Index]->tbl_Treetops.Rows[0]->Cells[cou_Cell]->Data[0]).U * 100) + cou_Index));
               }
          }
     }
     
     //Gathers the treetops and charges from the evaluated constructs into the MSC. Left.
     void gather_MSC_Eval_Input_L()
     {
          //*cout << "\n\t gather_MSC_Eval_Input_L()";
          
          int tmp_Input_Depth = 0;
          int tmp_Input_Depth_New = 0;
          
          //*cout << " -Gather_Treetops";
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               if (Constructs[cou_Index]->tbl_Treetops_Output_L.Number_Of_Rows == 0){ continue; }
               tmp_Input_Depth = 0;
               tmp_Input_Depth_New = 0;
               
               //*cout << "\n\n\n ["; oint(0, 13, cou_Index); cout << "]";
               for (int cou_Row=0;cou_Row<Constructs[cou_Index]->tbl_Treetops_Output_L.Number_Of_Rows;cou_Row++)
               {
                    for (int cou_Cell=0;cou_Cell<Constructs[cou_Index]->tbl_Treetops_Output_L.Rows[cou_Row]->Depth;cou_Cell++)
                    {
                         //if ((Constructs[cou_Index]->tbl_Treetops_Output_L.Rows[cou_Row]->Cells[cou_Cell]->Data[0]).U == 0){ continue; }
                         MSC.tbl_Input_Aux_L.add_Data_Int(0, cou_Row, (((Constructs[cou_Index]->tbl_Treetops_Output_L.Rows[cou_Row]->Cells[cou_Cell]->Data[0]).U * 100) + cou_Index));
                         MSC.tbl_Input_Charges_L.add_Data_Int(0, cou_Row, (((Constructs[cou_Index]->tbl_Charge_Output_L.Rows[cou_Row]->Cells[cou_Cell]->Data[0]).U)));
                         MSC.tbl_Input_Position_L.add_Data_Int(0, cou_Row, (((Constructs[cou_Index]->tbl_Position_Output_L.Rows[cou_Row]->Cells[cou_Cell]->Data[0]).U)));
                         //*cout << "   ("; oint(0, 14, (((Constructs[cou_Index]->tbl_Treetops_Output_L.Rows[cou_Row]->Cells[cou_Cell]->Data[0]).U * 100) + cou_Index));
                         //*cout << ")  ("; oint(0, 7, (((Constructs[cou_Index]->tbl_Position_Output_L.Rows[cou_Row]->Cells[cou_Cell]->Data[0]).U))); cout << ")";
                    }
               }
          }
          
          //*cout << " -Set_MSC_Depths";
          if (MSC.tbl_Input_Position_L.Number_Of_Rows == 0){ return; }
          
          //Gets the highest input depth needed for building.
          for (int cou_Cell=0;cou_Cell<MSC.tbl_Input_Position_L.Rows[0]->Depth;cou_Cell++)
          {
               tmp_Input_Depth = 0;
               tmp_Input_Depth_New = 0;
               for (int cou_D=0;cou_D<MSC.tbl_Input_Position_L.Rows[0]->Cells[cou_Cell]->Length;cou_D++)
               {
                    tmp_Input_Depth_New = (MSC.tbl_Input_Position_L.Rows[0]->Cells[cou_Cell]->Data[cou_D]).I + 1;
                    if (tmp_Input_Depth < tmp_Input_Depth_New){ tmp_Input_Depth = tmp_Input_Depth_New; }
               }
               MSC.tbl_Input_Depths_L.add_Data_Int(0, cou_Cell, tmp_Input_Depth);
          }
     }
     
     //Gathers the treetops and charges from the evaluated constructs into the MSC. Right.
     void gather_MSC_Eval_Input_R()
     {
          //*cout << "\n\t gather_MSC_Eval_Input_R()";
          
          int tmp_Input_Depth = 0;
          int tmp_Input_Depth_New = 0;
          
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               if (Constructs[cou_Index]->tbl_Treetops_Output_R.Number_Of_Rows == 0){ continue; }
               tmp_Input_Depth = 0;
               tmp_Input_Depth_New = 0;
               
               for (int cou_Row=0;cou_Row<Constructs[cou_Index]->tbl_Treetops_Output_R.Number_Of_Rows;cou_Row++)
               {
                    for (int cou_Cell=0;cou_Cell<Constructs[cou_Index]->tbl_Treetops_Output_R.Rows[cou_Row]->Depth;cou_Cell++)
                    {
                         //if ((Constructs[cou_Index]->tbl_Treetops_Output_R.Rows[cou_Row]->Cells[cou_Cell]->Data[0]).U == 0){ continue; }
                         MSC.tbl_Input_Aux_R.add_Data_Int(0, cou_Row, (((Constructs[cou_Index]->tbl_Treetops_Output_R.Rows[cou_Row]->Cells[cou_Cell]->Data[0]).U * 100) + cou_Index));
                         MSC.tbl_Input_Charges_R.add_Data_Int(0, cou_Row, (((Constructs[cou_Index]->tbl_Charge_Output_R.Rows[cou_Row]->Cells[cou_Cell]->Data[0]).U)));
                         MSC.tbl_Input_Position_R.add_Data_Int(0, cou_Row, (((Constructs[cou_Index]->tbl_Position_Output_R.Rows[cou_Row]->Cells[cou_Cell]->Data[0]).U)));
                    }
               }
          }
          
          if (MSC.tbl_Input_Position_R.Number_Of_Rows == 0){ return; }
          
          for (int cou_Cell=0;cou_Cell<MSC.tbl_Input_Position_R.Rows[0]->Depth;cou_Cell++)
          {
               tmp_Input_Depth = 0;
               tmp_Input_Depth_New = 0;
               for (int cou_D=0;cou_D<MSC.tbl_Input_Position_R.Rows[0]->Cells[cou_Cell]->Length;cou_D++)
               {
                    tmp_Input_Depth_New = (MSC.tbl_Input_Position_R.Rows[0]->Cells[cou_Cell]->Data[cou_D]).I + 1;
                    if (tmp_Input_Depth < tmp_Input_Depth_New){ tmp_Input_Depth = tmp_Input_Depth_New; }
               }
               MSC.tbl_Input_Depths_R.add_Data_Int(0, cou_Cell, tmp_Input_Depth);
          }
     }
     
     //Resizes the constructs array to accept the new inputs.
     void resize(int p_Size)
     {
          if (Construct_Count >= p_Size){ return; }
          
          //Setup the tmp rows array.
          c_NT3_Construct_1D ** tmp_Constructs;
          
          tmp_Constructs = new c_NT3_Construct_1D*[Construct_Count];
          
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               tmp_Constructs[cou_Index] = Constructs[cou_Index];
               Constructs[cou_Index] = NULL;
          }
          
          delete [] Constructs;
          Constructs = NULL;
          Constructs = new c_NT3_Construct_1D*[p_Size];
          
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               Constructs[cou_Index] = tmp_Constructs[cou_Index];
               tmp_Constructs[cou_Index] = NULL;
          }
          
          delete [] tmp_Constructs;
          tmp_Constructs = NULL;
          
          string tmp_Construct_Name = "";
          
          for (int cou_Index=Construct_Count;cou_Index<p_Size;cou_Index++)
          {
               Constructs[cou_Index] = NULL;
               Constructs[cou_Index] = new c_NT3_Construct_1D;
               Constructs[cou_Index]->tbl_Input.set_Row(0, tbl_Input.Rows[cou_Index]); 
               tmp_Construct_Name = Name + "___Input_" + int2str(cou_Index);
               Constructs[cou_Index]->set_Name(tmp_Construct_Name);
               Constructs[cou_Index]->settings_Tier_Is_Floating = 1;
               Constructs[cou_Index]->settings_Tier_Depth = .9;
               Constructs[cou_Index]->settings_Cross_CAN_Charging = 1;
          }
          
          Construct_Count = p_Size;
          
          //Resize the construct output tracker.
          delete [] Construct_Has_Output;
          Construct_Has_Output = NULL;
          Construct_Has_Output = new int[Construct_Count];
          
          delete [] Vision_Output_Count;
          Vision_Output_Count = NULL;
          Vision_Output_Count = new int[Construct_Count];
          
          delete [] Vision_Charge;
          Vision_Charge = NULL;
          Vision_Charge = new long long int[Construct_Count];
          
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               Construct_Has_Output[cou_Index] = 0;
               Vision_Output_Count[cou_Index] = 0;
               Vision_Charge[cou_Index] = 0;
          }
     }
     
     //Resets the input and output tables.
     void reset()
     {
          tbl_Input.soft_Reset();
          tbl_Input_Goal.soft_Reset();
          
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               Constructs[cou_Index]->reset_Outputs();
               Construct_Has_Output[cou_Index] = 0;
               Vision_Output_Count[cou_Index] = 0;
               Vision_Charge[cou_Index] = 0;
          }
          
          MSC.reset_Inputs();
          MSC.reset_Outputs();
          
     }
     
     
     
     
     
     ////==---------------------------+
     //==--   RAW CONSTRUCT SETTINGS
     ////==---------------------------+
     
     void set_Left_Leg(bool p_Setting, bool p_MSC = 0)
     {  
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               if (p_MSC > 1){ continue; } if (Constructs[cou_Index] != NULL){ Constructs[cou_Index]->setting_Charge_L = p_Setting; }
          }
          if (p_MSC){ MSC.setting_Charge_L = p_Setting; }
     }
     
     void set_Right_Leg(bool p_Setting, bool p_MSC = 0)
     {  
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               if (p_MSC > 1){ continue; } if (Constructs[cou_Index] != NULL){ Constructs[cou_Index]->setting_Charge_R = p_Setting; }
          }
          if (p_MSC){ MSC.setting_Charge_R = p_Setting; }
     }
     
     void set_Tier_Floating(double p_Setting, bool p_MSC = 0)
     {  
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               if (p_MSC > 1){ continue; } if (Constructs[cou_Index] != NULL){ Constructs[cou_Index]->settings_Tier_Depth = p_Setting; }
          }
          if (p_MSC){ MSC.settings_Tier_Depth = p_Setting; }
     }
     
     void set_Tier_Is_Floating(int p_Setting, bool p_MSC = 0)
     {  
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               if (p_MSC > 1){ continue; } if (Constructs[cou_Index] != NULL){ Constructs[cou_Index]->settings_Tier_Is_Floating = p_Setting; }
          }
          if (p_MSC){ MSC.settings_Tier_Is_Floating = p_Setting; }
     }
     
     void set_Tier(int p_Setting, bool p_MSC = 0)
     {  
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               if (p_MSC > 1){ continue; } if (Constructs[cou_Index] != NULL){ Constructs[cou_Index]->settings_Tier = p_Setting; }
          }
          if (p_MSC){ MSC.settings_Tier = p_Setting; }
     }
     
     void set_Action_Potential_Threshold(double p_Setting, bool p_MSC = 0)
     {  
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               if (p_MSC > 1){ continue; } if (Constructs[cou_Index] != NULL){ Constructs[cou_Index]->settings_Action_Potential_Threshold = p_Setting; }
          }
          if (p_MSC){ MSC.settings_Action_Potential_Threshold = p_Setting; }
     }
     void set_Base_Charge(int p_Setting, bool p_MSC = 0)
     {  
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               if (p_MSC > 1){ continue; } if (Constructs[cou_Index] != NULL){ Constructs[cou_Index]->settings_Base_Charge = p_Setting; }
          }
          if (p_MSC){ MSC.settings_Base_Charge = p_Setting; }
     }
     void set_Modifier_Charge(double p_Setting, bool p_MSC = 0)
     {  
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               if (p_MSC > 1){ continue; } if (Constructs[cou_Index] != NULL){ Constructs[cou_Index]->settings_Modifier_Charge = p_Setting; }
          }
          if (p_MSC){ MSC.settings_Modifier_Charge = p_Setting; }
     }
     void set_Input_Charging_Depth(double p_Setting, bool p_MSC = 0)
     {  
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               if (p_MSC > 1){ continue; } if (Constructs[cou_Index] != NULL){ Constructs[cou_Index]->settings_Input_Charging_Depth = p_Setting; }
          }
          if (p_MSC){ MSC.settings_Input_Charging_Depth = p_Setting; }
     }
     void set_Sauto(int p_Setting, bool p_MSC = 0)
     {  
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               if (p_MSC > 1){ continue; } if (Constructs[cou_Index] != NULL){ Constructs[cou_Index]->sauto_Use_RC = p_Setting; }
          }
          if (p_MSC){ MSC.sauto_Use_RC = p_Setting; }
     }
     
     void set_Charge_Spike(int p_Setting, bool p_MSC = 0)
     {  
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               if (p_MSC > 1){ continue; } if (Constructs[cou_Index] != NULL){ Constructs[cou_Index]->setting_Charge_Spike = p_Setting; }
          }
          if (p_MSC){ MSC.setting_Charge_Spike = p_Setting; }
     }
     
     void set_Sauto_C(int p_Setting, int p_Construct = 0)
     {  
          if (Constructs[p_Construct] != NULL){ Constructs[p_Construct]->sauto_Use_RC = p_Setting; }
     }
     
     void set_Sauto_Type(int p_Setting, bool p_MSC = 0)
     {  
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               if (p_MSC > 1){ continue; }
               if (Constructs[cou_Index] != NULL){ Constructs[cou_Index]->sauto_Type = p_Setting; }
          }
          if (p_MSC){ MSC.sauto_Type = p_Setting; }
     }
     
     void set_Sauto_Type_C(int p_Index, int p_Setting)
     {  
          if (p_Index >= Construct_Count){ return; }
          if (Constructs[p_Index] != NULL){ Constructs[p_Index]->sauto_Type = p_Setting; }
     }
     
     void set_Left_Leg_C(int p_Index, bool p_Setting)
     {  
          if (p_Index >= Construct_Count){ return; }
          if (Constructs[p_Index] != NULL){ Constructs[p_Index]->setting_Charge_L = p_Setting; }
     }
     
     void set_Right_Leg_C(int p_Index, bool p_Setting)
     {  
          if (p_Index >= Construct_Count){ return; }
          if (Constructs[p_Index] != NULL){ Constructs[p_Index]->setting_Charge_R = p_Setting; }
     }
     
     void set_Tier_Is_Floating_C(int p_Index, int p_Setting)
     {  
          if (p_Index >= Construct_Count){ return; }
          if (Constructs[p_Index] != NULL){ Constructs[p_Index]->settings_Tier_Is_Floating = p_Setting; }
     }
     
     void set_Tier_Floating_C(int p_Index, double p_Setting)
     {  
          if (p_Index >= Construct_Count){ return; }
          if (Constructs[p_Index] != NULL){ Constructs[p_Index]->settings_Tier_Depth = p_Setting; }
     }
     
     void set_Tier_C(int p_Index, int p_Setting)
     {  
          if (Constructs[p_Index] != NULL){ Constructs[p_Index]->settings_Tier = p_Setting; }
     }
     
     void set_Action_Potential_Threshold_C(int p_Index, double p_Setting)
     {  
          if (p_Index >= Construct_Count){ return; }
          if (Constructs[p_Index] != NULL){ Constructs[p_Index]->settings_Action_Potential_Threshold = p_Setting; }
     }
     void set_Base_Charge_C(int p_Index, int p_Setting)
     {  
          if (p_Index >= Construct_Count){ return; }
          if (Constructs[p_Index] != NULL){ Constructs[p_Index]->settings_Base_Charge = p_Setting; }
     }
     void set_Modifier_Charge_C(int p_Index, double p_Setting)
     {  
          if (p_Index >= Construct_Count){ return; }
          if (Constructs[p_Index] != NULL){ Constructs[p_Index]->settings_Modifier_Charge = p_Setting; }
     }
     void set_Input_Charging_Depth_C(int p_Index, double p_Setting)
     {  
          if (p_Index >= Construct_Count){ return; }
          if (Constructs[p_Index] != NULL){ Constructs[p_Index]->settings_Input_Charging_Depth = p_Setting; }
     }
     
     void set_Charge_Spike_C(int p_Index, int p_Setting)
     {  
          if (p_Index >= Construct_Count){ return; }
          if (Constructs[p_Index] != NULL){ Constructs[p_Index]->setting_Charge_Spike = p_Setting; }
     }
     
     
     ////==-----------+
     //==--   VISION
     ////==-----------+
     
     //Gets whether or not a given construct has output.
     int get_Construct_Has_Output(int p_Construct)
     {
          if (p_Construct >= Construct_Count){ return 0; }
          
          return Construct_Has_Output[p_Construct];
     }
     
     //Determines the deviance of the current vision cell.
     double gather_Vision_Deviance(c_Raw_Table_Cell_1D * p_Cell_Vision, c_Raw_Table_Cell_1D * p_Cell_Input)
     {
          
          if (p_Cell_Vision == NULL || p_Cell_Input == NULL){ return 5000; }
          double tmp_Len = p_Cell_Input->Length;
          double tmp_Deviance = 0;
          
          //cout << "\n\n\n I->"; p_Cell_Input->output_F(); cout << "                                   ";
          //cout << "\n V->"; p_Cell_Vision->output_F();  cout << "                                   ";
          
          if (tmp_Len >= p_Cell_Vision->Length){ return 5000; }
          
          //Loop through the data in the cells doing a comparision.
          for (int cou_D=0;cou_D<tmp_Len;cou_D++)
          {
               tmp_Deviance += absoluted(get_Difference(p_Cell_Vision->Data[cou_D].F, p_Cell_Input->Data[cou_D].F));
          }
          tmp_Deviance /= tmp_Len;
          
          /*
          c_Shipyard_Line_Graph tmp_Graph;
          for (int cou_D=0;cou_D<tmp_Len;cou_D++)
          {
               tmp_Graph.submit_Data_Point(0, cou_D, p_Cell_Input->Data[cou_D].F);
               tmp_Graph.submit_Data_Point(1, cou_D, p_Cell_Vision->Data[cou_D].F);
          }
          */
          //cout << "\n";
          //tmp_Graph.output_Minimal(get_Console_Cursor_X(), get_Console_Cursor_Y(), 10, 5);
          
          //cout << "\n ->"; odbl(0, 13, tmp_Deviance);
          //system("PAUSE > NULL");
          
          
          return tmp_Deviance;
     }
     
     //Determines the deviance of the current cell containing a string.
     double gather_Vision_Deviance_A(c_Raw_Table_Cell_1D * p_Cell_Vision, c_Raw_Table_Cell_1D * p_Cell_Input)
     {
          if (p_Cell_Vision == NULL || p_Cell_Input == NULL){ return 5000; }
          double tmp_Len = p_Cell_Input->Length;
          double tmp_Deviance = 0;
          /*
          cout << "\n\n\n I->"; p_Cell_Input->output_C(); cout << "                                   ";
          cout << "\n V->"; p_Cell_Vision->output_C();  cout << "                                   ";
          */
          //Loop through the data in the cells doing a comparision.
          for (int cou_D=0;cou_D<tmp_Len;cou_D++)
          {
               if (absoluted(get_Difference(p_Cell_Vision->Data[cou_D].I, p_Cell_Input->Data[cou_D].I)))
               {
                    tmp_Deviance += 1;
               }
               else 
               {
                    tmp_Deviance += 0;
               }
          }
          tmp_Deviance /= tmp_Len;
          
          /*
          c_Shipyard_Line_Graph tmp_Graph;
          for (int cou_D=0;cou_D<tmp_Len;cou_D++)
          {
               tmp_Graph.submit_Data_Point(0, cou_D, p_Cell_Input->Data[cou_D].I);
               tmp_Graph.submit_Data_Point(1, cou_D, p_Cell_Vision->Data[cou_D].I);
          }
          
          cout << "\n";
          tmp_Graph.output_Minimal(get_Console_Cursor_X(), get_Console_Cursor_Y(), 10, 5);
          
          cout << "\n ->"; odbl(0, 13, tmp_Deviance);
          system("PAUSE > NULL");
          */
          return tmp_Deviance;
     }
     
     //Gathers a weighted average vision.
     void gather_Vision_Cell(c_Raw_Table_Cell_1D * p_Cell_Vision, c_Raw_Table_Cell_1D * p_Cell_Output, double p_Charge, int p_Position = 0)
     {
          if (p_Cell_Vision == NULL || p_Cell_Output == NULL){ return; }
          int tmp_Len = p_Cell_Output->Length;
          
          //Loop through the data in the cells doing a comparision.
          for (int cou_D=0;cou_D<tmp_Len;cou_D++)
          {
               p_Cell_Vision->Data[cou_D].F += (p_Cell_Output->Data[cou_D].F * p_Charge);
          }
          
          return;
     }
     
     c_Raw_Table_Cell_1D * gather_Vision_Row_A(int p_Row, int p_Cell, int p_Gather_Verified_Vision = 1)
     {
          //Bounds checking
          if (p_Row >= Construct_Count){ return NULL; }
          
          //Get the goal cell and the output row.
          c_Raw_Table_Row_1D * output_Row = Constructs[p_Row]->tbl_Pattern_Output_L.get_Row_Ref(p_Cell);
          c_Raw_Table_Row_1D * output_Charge_Row = Constructs[p_Row]->tbl_Charge_Output_L.get_Row_Ref(p_Cell);
          c_Raw_Table_Row_1D * output_Verified_Row = tbl_Verified_Output.get_Row_Ref(p_Cell);
          c_Raw_Table_Cell_1D * Vision_Cell = tbl_Vision_Output.get_Cell_Ref_Hard(p_Row, 0);
          
          Vision_Cell->reset();
          
          //If there are no charges return;
          if (output_Row == NULL){ return NULL; }
          if (output_Charge_Row == NULL){ return NULL; }
          
          for (int cou_C=0;cou_C<output_Charge_Row->Depth;cou_C++)
          {
               if (output_Verified_Row->Cells[cou_C]->Data[0].I == 0){ continue; }
               if (gather_Vision_Deviance_A(output_Row->Cells[cou_C], tbl_Input.get_Cell_Ref_Hard(p_Row, 0)) == 1.0f){ output_Verified_Row->Cells[cou_C]->Data[0].I = 0; continue; }
          }
          return NULL;
     }
     
     //Gathers the stats for a row.
     c_Raw_Table_Cell_1D * gather_Vision_Row_I(int p_Row, int p_Cell, int p_Gather_Verified_Vision = 1)
     {
          //Bounds checking
          if (p_Row >= Construct_Count){ return NULL; }
          
          //The highest charge in the row, used for getting the weighted averages to add.
          long long int tmp_Total_Charge = 0;
          
          //Get the goal cell and the output row.
          c_Raw_Table_Row_1D * output_Row = Constructs[p_Row]->tbl_Pattern_Output_L.get_Row_Ref(p_Cell);
          c_Raw_Table_Row_1D * output_Charge_Row = Constructs[p_Row]->tbl_Charge_Output_L.get_Row_Ref(p_Cell);
          c_Raw_Table_Row_1D * output_Position_Row = Constructs[p_Row]->tbl_Position_Output_L.get_Row_Ref(p_Cell);
          c_Raw_Table_Row_1D * output_Verified_Row = tbl_Verified_Output.get_Row_Ref(p_Cell);
          c_Raw_Table_Cell_1D * Vision_Cell = tbl_Vision_Output.get_Cell_Ref_Hard(p_Row, 0);
          
          Vision_Cell->reset();
          
          //If there are no charges return;
          if (output_Row == NULL){ return NULL; }
          if (output_Charge_Row == NULL){ return NULL; }
          /*
          cout << "\n";
          cout << "                                                                                                 ";
          cout << "\n";
          output_Row->output_F();
          cout << "                                                                                                 ";
          cout << "\n";
          output_Charge_Row->output_F();
          cout << "                                                                                                 ";
          cout << "\n";
          output_Verified_Row->output_I();
          cout << "\n";
          cout << "                                                                                                 ";
          cout << "\n";
          */
          //Gather the total charge.
          for (int cou_C=0;cou_C<output_Charge_Row->Depth;cou_C++)
          {
               if (output_Verified_Row->Cells[cou_C]->Data[0].I == 0){ continue; }
               if (gather_Vision_Deviance(output_Row->Cells[cou_C], tbl_Input.get_Cell_Ref_Hard(p_Row, 0)) >= settings_Vision_Fog){ output_Verified_Row->Cells[cou_C]->Data[0].I = 0; continue; }
               tmp_Total_Charge += output_Charge_Row->Cells[cou_C]->Data[0].I;
          }
          
          if (tmp_Total_Charge == 0){ return NULL; }
          
          //Sets the vision charge.
          tbl_Vision_Output_Charge.set_Int(p_Row, 0, tmp_Total_Charge);
          
          //If either are NULL return;
          if (output_Row == NULL){ return NULL; }
          
          if (output_Row->Depth >= 1)
          {
               Vision_Cell->resize(output_Row->Cells[0]->Length);
          }
          for (int cou_C=0;cou_C<output_Row->Depth;cou_C++)
          {
               if (output_Verified_Row->Cells[cou_C]->Data[0].I == 0){ continue; }
               gather_Vision_Cell(Vision_Cell, output_Row->Cells[cou_C], (double (output_Charge_Row->Cells[cou_C]->Data[0].I) / tmp_Total_Charge), output_Position_Row->Cells[cou_C]->Data[0].I);
               Vision_Output_Count[p_Row]++;
          }
          Vision_Charge[p_Row] = tmp_Total_Charge;
          
          return Vision_Cell;
     }
     
     //Gathers the stats for a row.
     c_Raw_Table_Cell_1D * gather_Vision_Row_F(int p_Row, int p_Cell, int p_Gather_Verified_Vision = 1)
     {
          //Bounds checking
          if (p_Row >= Construct_Count){ return NULL; }
          
          //The highest charge in the row, used for getting the weighted averages to add.
          long long int tmp_Total_Charge = 0;
          
          //Get the goal cell and the output row.
          c_Raw_Table_Row_1D * output_Row = Constructs[p_Row]->tbl_Pattern_Output_L.get_Row_Ref(p_Cell);
          c_Raw_Table_Row_1D * output_Charge_Row = Constructs[p_Row]->tbl_Charge_Output_L.get_Row_Ref(p_Cell);
          c_Raw_Table_Row_1D * output_Position_Row = Constructs[p_Row]->tbl_Position_Output_L.get_Row_Ref(p_Cell);
          c_Raw_Table_Row_1D * output_Verified_Row = tbl_Verified_Output.get_Row_Ref(p_Row);
          c_Raw_Table_Cell_1D * Vision_Cell = tbl_Vision_Output.get_Cell_Ref_Hard(p_Row, 0);
          
          Vision_Cell->reset();
          
          //If there are no charges return;
          if (output_Row == NULL){ return NULL; }
          if (output_Charge_Row == NULL){ return NULL; }
          
          //Gather the total charge.
          for (int cou_C=0;cou_C<output_Charge_Row->Depth;cou_C++)
          {
               if (output_Verified_Row->Cells[cou_C]->Data[0].I == 0){ continue; }
               if (gather_Vision_Deviance(output_Row->Cells[cou_C], tbl_Input.get_Cell_Ref_Hard(p_Row, 0)) >= settings_Vision_Fog){ output_Verified_Row->Cells[cou_C]->Data[0].I = 0; continue; }
               tmp_Total_Charge += output_Charge_Row->Cells[cou_C]->Data[0].I;
          }
          
          if (tmp_Total_Charge == 0){ return NULL; }
          
          //Sets the vision charge.
          tbl_Vision_Output_Charge.set_Int(p_Row, 0, tmp_Total_Charge);
          
          if (output_Row->Depth >= 1)
          {
               Vision_Cell->reset();
               Vision_Cell->resize(output_Row->Cells[0]->Length);
          }
          for (int cou_C=0;cou_C<output_Row->Depth;cou_C++)
          {
               if (output_Verified_Row->Cells[cou_C]->Data[0].I == 0){ continue; }
               gather_Vision_Cell(Vision_Cell, output_Row->Cells[cou_C], (double (output_Charge_Row->Cells[cou_C]->Data[0].I) / tmp_Total_Charge), output_Position_Row->Cells[cou_C]->Data[0].I);
               Vision_Output_Count[p_Row]++;
          }
          Vision_Charge[p_Row] = tmp_Total_Charge;
          
          return Vision_Cell;
     }
     
     
     c_Raw_Table_Cell_1D * gather_Vision_Row(int p_Row, int p_Cell, int p_Gather_Verified_Vision = 1)
     {
          //Bounds checking
          if (p_Row >= Construct_Count){ return NULL; }
          //cout << "\n gather_Vision_Row(" << p_Row << ", " << p_Cell << ") Constructs[" << p_Row << "]->sauto_Type = " << Constructs[p_Row]->sauto_Type;
          if (Constructs[p_Row]->sauto_Type == 0){ /*cout << " I ";*/ return gather_Vision_Row_I(p_Row, p_Cell); }
          if (Constructs[p_Row]->sauto_Type == 1){ /*cout << " F ";*/ return gather_Vision_Row_F(p_Row, p_Cell); }
          if (Constructs[p_Row]->sauto_Type == 2){ /*cout << " A ";*/ return gather_Vision_Row_A(p_Row, p_Cell); }
          //ostr(0, 12, " FAIL --NULL RETURNED...");
          return NULL;
     }
     
     
     //Gathers the output count for a given row.
     int gather_Output_Count_For_Given_Row(int p_Row)
     {
          //Bounds checking
          if (p_Row >= Construct_Count){ return 0; }
          
          //Get the goal cell and the output row.
          c_Raw_Table_Row_1D * output_Row = Constructs[p_Row]->tbl_Pattern_Output_L.get_Row_Ref(0);
          
          //If there are no charges return;
          if (output_Row == NULL){ return 0; }
          
          return output_Row->Depth;
     }
     
     ////==--------------------+
     //==--   ACCURACY GATHERING
     ////==--------------------+
     
     //Gather percentages for two cells.
     double gather_Percentage(c_Raw_Table_Cell_1D * p_Cell_Goal, c_Raw_Table_Cell_1D * p_Cell_Output)
     {
          if (p_Cell_Goal == NULL || p_Cell_Output == NULL){ return 0.0f; }
          double tmp_Accuracy = 0.0f;
          int tmp_Len = p_Cell_Goal->Length;
          if (tmp_Len > p_Cell_Output->Length){ tmp_Len = p_Cell_Output->Length; }
          
          //Loop through the data in the cells doing a comparision.
          for (int cou_D=0;cou_D<tmp_Len;cou_D++)
          {
               tmp_Accuracy += absolutef(get_Percent_Change_F(p_Cell_Goal->Data[cou_D].F, p_Cell_Output->Data[cou_D].F));
          }
          
          return tmp_Accuracy;
     }
     
     double gather_Accuracy_Row_A(int p_Row, int p_Cell)
     {
          return 0;
     }
     
     
     //-- Gathers accuracy from the output tables.
     
     //Gathers the stats for a row.
     double gather_Accuracy_Row_I(int p_Row, int p_Cell)
     {
          //Bounds checking
          if (p_Row >= Construct_Count){ return 5000.0f;; }
          
          //The temporary accuracy counter.
          double tmp_Acc = 0.0f;
          
          //The highest charge in the row, used for getting the weighted averages to add.
          long long int tmp_Total_Charge = 0;
          
          //Get the goal cell and the output row.
          c_Raw_Table_Cell_1D * goal_Cell = tbl_Input_Goal.get_Cell_Ref(p_Row, p_Cell);
          c_Raw_Table_Row_1D * output_Row = Constructs[p_Row]->tbl_Pattern_Output_L.get_Row_Ref(p_Cell);
          c_Raw_Table_Row_1D * output_Charge_Row = Constructs[p_Row]->tbl_Charge_Output_L.get_Row_Ref(p_Cell);
          
          //If there are no charges return;
          if (output_Row == NULL){ return 5000.0f;; }
          if (output_Charge_Row == NULL){ return 5000.0f;; }
          
          /*
          cout << "\n\n\n\n Construct->" << Constructs[p_Row]->Name;
          cout << "\n G->";
          goal_Cell->output_I();
          cout << "\n OR->";
          output_Row->output_I();
          cout << "\n OCR->";
          output_Charge_Row->output_I();
          */
          
          //Gather the total charge.
          for (int cou_C=0;cou_C<output_Charge_Row->Depth;cou_C++)
          {
               tmp_Total_Charge += output_Charge_Row->Cells[cou_C]->Data[0].I;
          }
          
          if (tmp_Total_Charge == 0){ return 5000.0f;; }
          
          //If either are NULL return;
          if (goal_Cell == NULL || output_Row == NULL){ return 5000.0f;; }
          
          for (int cou_C=0;cou_C<output_Row->Depth;cou_C++)
          {
               /*
               cout << "\n\t " << cou_C; cout.flush();
               cout << " " << goal_Cell;
               cout << " oR->C[" << cou_C << "] D->" << output_Row->Cells[cou_C]->Length << " " << output_Row->Cells[cou_C]; cout.flush();
               cout << " oCR->D->" << output_Charge_Row->Depth; cout.flush();
               cout << " oCR->Cells[" << cou_C << "]" << output_Charge_Row->Cells[cou_C]; cout.flush();
               cout << " oCR->Cells[" << cou_C << "]->Length ->" << output_Charge_Row->Cells[cou_C]->Length; cout.flush();
               cout << " oCR->Cells[" << cou_C << "]->Data[0].I ->" << output_Charge_Row->Cells[cou_C]->Data[0].I; cout.flush();
                * */
               tmp_Acc += (gather_Percentage(goal_Cell, output_Row->Cells[cou_C]) * (double (output_Charge_Row->Cells[cou_C]->Data[0].I) / tmp_Total_Charge));
          }
          
          return tmp_Acc;
     }
     
     double gather_Accuracy_Row(int p_Row, int p_Cell)
     {
          //Bounds checking
          if (p_Row >= Construct_Count){ return 5000.0f;; }
          
          if (Constructs[p_Row]->sauto_Type == 0){ return gather_Accuracy_Row_I(p_Row, p_Cell); }
          if (Constructs[p_Row]->sauto_Type == 1){ return gather_Accuracy_Row_A(p_Row, p_Cell); }
          return 5000.0f;;
     }
     
     //Used for gathering the accuracy of a given row through higher level calling.
     double gather_Accuracy_Row(int p_Row)
     {
          if (tbl_Input.Number_Of_Rows <= p_Row){ return 5000.0f;; }
          double tmp_Acc = 0.0f;
          for (int cou_C=0;cou_C<tbl_Input.Rows[p_Row]->Depth;cou_C++)
          {
               tmp_Acc += gather_Accuracy_Row(p_Row, cou_C);
          }
          return tmp_Acc;
     }
     
     //Gather accuracy for every row.
     double gather_Accuracy(int p_Limit = 0)
     {
          //The tmp vars
          double tmp_Acc = 0.0f;
          
          //The number of inputs to gather accuracy from.
          int tmp_Limit = tbl_Input.Number_Of_Rows;
          
          //Checks for a limit on the items to gather accuracy from.
          if (settings_Accuracy_Upper != -1)
          {
               if (settings_Accuracy_Upper < tmp_Limit)
               {
                    tmp_Limit = settings_Accuracy_Upper;
               }
          }
          
          if ((settings_Auto_Row_Limit) && (settings_Auto_Row_Limit <= tmp_Limit)){ tmp_Limit = settings_Auto_Row_Limit; }
          if ((p_Limit > 0) && (p_Limit < tmp_Limit)){ tmp_Limit = p_Limit; }
          
          //For every row loop through all the cells gathering the accuracy.
          for (int cou_R=0;cou_R<tmp_Limit;cou_R++)
          {
               for (int cou_C=0;cou_C<tbl_Input.Rows[cou_R]->Depth;cou_C++)
               {
                    tmp_Acc += gather_Accuracy_Row(cou_R, cou_C);
               }
          }
          return (tmp_Acc / double (tmp_Limit));
     }
     
     
     //-- Gathers accuracy from vision rather than the outputs.
     
     //Gathers the stats for a row.
     double gather_VAccuracy_Row_I(int p_Row, int p_Cell)
     {
          //Bounds checking
          if (p_Row >= Construct_Count){ return 5000.0f;; }
          
          //The temporary accuracy counter.
          double tmp_Acc = 0.0f;
          
          //Get the goal cell and the output row.
          c_Raw_Table_Cell_1D * goal_Cell = tbl_Input_Goal.get_Cell_Ref(p_Row, p_Cell);
          c_Raw_Table_Cell_1D * vision_Cell = tbl_Vision_Output.get_Cell_Ref(p_Row, 0);
          
          //Nullcheck
          if (goal_Cell == NULL){ return 5000.0f;; }
          if (vision_Cell == NULL){ return 5000.0f;; }
          
          tmp_Acc += (gather_Percentage(goal_Cell, vision_Cell));
          
          return tmp_Acc;
     }
     
     //Gathers the stats for a row.
     double gather_VAccuracy_Row_A(int p_Row, int p_Cell)
     {
          return 0.0f;
     }
     
     double gather_VAccuracy_Row(int p_Row, int p_Cell)
     {
          //Bounds checking
          if (p_Row >= Construct_Count){ return 5000.0f;; }
          
          if (Constructs[p_Row]->sauto_Type == 0){ return gather_VAccuracy_Row_I(p_Row, p_Cell); }
          if (Constructs[p_Row]->sauto_Type == 1){ return gather_VAccuracy_Row_A(p_Row, p_Cell); }
          return 999.99;
     }
     
     //Used for gathering the accuracy of a given row through higher level calling.
     double gather_VAccuracy_Row(int p_Row)
     {
          if (tbl_Input.Number_Of_Rows <= p_Row){ return 5000.0f;; }
          
          return gather_VAccuracy_Row(p_Row, 0);
     }
     
     //Gather accuracy for every row.
     double gather_VAccuracy(int p_Limit = 0)
     {
          //The tmp vars
          double tmp_Acc = 0.0f;
          double tmp_Total_Charge = 0.0f;
          
          //The number of inputs to gather accuracy from.
          int tmp_Limit = tbl_Input.Number_Of_Rows;
          
          if ((settings_Auto_Row_Limit) && (settings_Auto_Row_Limit <= tmp_Limit)){ tmp_Limit = settings_Auto_Row_Limit; }
          if ((p_Limit > 0) && (p_Limit < tmp_Limit)){ tmp_Limit = p_Limit; }
          
          //For every row loop through all the cells gathering the charge.
          for (int cou_R=0;cou_R<tmp_Limit;cou_R++)
          {
               
               //Gathers the vision for the row.
               gather_Vision_Row(cou_R, 0);
               
               tmp_Total_Charge += tbl_Vision_Output_Charge.get_F(cou_R, 0, 0);
          }
          
          if (tmp_Total_Charge == 0){ return 5000.0f;; }
          
          //For every row loop through all the cells gathering the accuracy.
          for (int cou_R=0;cou_R<tmp_Limit;cou_R++)
          {
               tmp_Acc += (gather_VAccuracy_Row(cou_R, 0) * (tbl_Vision_Output_Charge.get_F(cou_R, 0, 0) / tmp_Total_Charge));
          }
          return (tmp_Acc / double (tbl_Input.Number_Of_Rows));
     }
     
     
     
     ////==--------------------+
     //==--   AUTOCALIBRATION
     ////==--------------------+
     
     //Autocalicrates a given constructs APT
     void autocalibrate_Given_Construct_APT(c_NT3_Construct_1D * p_Con, double p_Inc, double p_LInc, int p_Timer = -1)
     {
          //tmp vars
          double tmp_Control_Acc = 0.0f;
          double tmp_Above_Acc = 0.0f;
          double tmp_Below_Acc = 0.0f;
          bool tmp_Done = false;
          string tmp_How_We_Doing = "Starting";
          double tmp_APT = 0.0f;
          double tmp_APT_Inc = p_Inc;
          int tmp_Timer = p_Timer;
          
          while(!tmp_Done)
          {
               tmp_Done = true;
               
               if (tmp_How_We_Doing == "Starting")
               {
                    //Get the current APT
                    tmp_APT = p_Con->settings_Action_Potential_Threshold;
                    
                    //Gather the control.
                    tmp_Control_Acc = gather_Accuracy();
                    
                    //Gather the above.
                    p_Con->settings_Action_Potential_Threshold = (tmp_APT + tmp_APT_Inc);
                    
                    Eval_Freely();
                    tmp_Above_Acc = gather_Accuracy();
                    
                    //Gather below.
                    p_Con->settings_Action_Potential_Threshold = (tmp_APT - tmp_APT_Inc);
                    if ((tmp_APT - tmp_APT_Inc) < 0){ p_Con->settings_Action_Potential_Threshold = 0; }
                    
                    Eval_Freely();
                    tmp_Below_Acc = gather_Accuracy();
                    
                    tmp_Done = false;
                    
                    //Reset the constructs APT so that the next run does not fail.
                    p_Con->settings_Action_Potential_Threshold = tmp_APT;
               }
               if (tmp_How_We_Doing == "Down")
               {
                    //Set the current APT
                    tmp_APT -= tmp_APT_Inc;
                    if (tmp_APT < 0){ tmp_APT = 0; }
                    
                    //The acc_High is not equal to the current low.
                    tmp_Above_Acc = tmp_Control_Acc;
                    
                    //The current control is now equal to the low
                    tmp_Control_Acc = tmp_Below_Acc;
                    
                    //Gather the new low.
                    p_Con->settings_Action_Potential_Threshold = (tmp_APT - tmp_APT_Inc);
                    if ((tmp_APT - tmp_APT_Inc) < 0){ p_Con->settings_Action_Potential_Threshold = 0; }
                    
                    Eval_Freely();
                    tmp_Below_Acc = gather_Accuracy();
                    
                    tmp_Done = false;
                    
                    //Reset the constructs APT so that the next run does not fail.
                    p_Con->settings_Action_Potential_Threshold = tmp_APT;
               }
               if (tmp_How_We_Doing == "Up")
               {
                    //Set the current APT
                    tmp_APT += tmp_APT_Inc;
                    
                    //The acc_High is not equal to the current control.
                    tmp_Below_Acc = tmp_Control_Acc;
                    
                    //Set the control to the current high
                    tmp_Control_Acc = tmp_Above_Acc;
                    
                    //Gather the new high.
                    p_Con->settings_Action_Potential_Threshold = (tmp_APT + tmp_APT_Inc);
                    
                    Eval_Freely();
                    tmp_Above_Acc = gather_Accuracy();
                    
                    tmp_Done = false;
                    
                    //Reset the constructs APT so that the next run does not fail.
                    p_Con->settings_Action_Potential_Threshold = tmp_APT;
               }
               if (tmp_How_We_Doing == "Reduction")
               {
                    tmp_APT_Inc /= 2;
                    if (tmp_APT_Inc < settings_APT_LInc){ break; }
                    
                    tmp_Done = false;
                    tmp_How_We_Doing = "Starting";
                    tmp_APT = p_Con->settings_Action_Potential_Threshold;
                    continue;
               }
               
               
               cout << "\n     A->";
               oflt(0, 15, tmp_Above_Acc);
               cout << " C->";
               oflt(0, 14, tmp_Control_Acc);
               cout << " B->";
               oflt(0, 13, tmp_Below_Acc);
               
               cout << " APT->";
               oflt(0, 12, tmp_APT);
               cout << " APT_I->";
               oflt(0, 11, tmp_APT_Inc);
               
               //If all results are equal then return.
               if (tmp_Control_Acc == tmp_Above_Acc && tmp_Above_Acc == tmp_Below_Acc){ tmp_Done = true; tmp_How_We_Doing = "Finished"; break; }
               
               //If the control has reached the current best position then reduce the increment and hone in more.
               if (tmp_Control_Acc <= tmp_Above_Acc || tmp_Control_Acc <= tmp_Below_Acc){ tmp_How_We_Doing = "Reduction"; }
               
               //If the one below the control is better move in that direction.
               if (tmp_Control_Acc > tmp_Below_Acc){ tmp_How_We_Doing = "Down"; }
               
               //If the above is better move in that direction, but only if it is also better than below.
               if (tmp_Control_Acc > tmp_Above_Acc && tmp_Above_Acc < tmp_Below_Acc){ tmp_How_We_Doing = "Up"; }
               
               if (tmp_How_We_Doing == "Down")
               {
                    if ((tmp_Control_Acc - tmp_Below_Acc) < .01){ tmp_How_We_Doing = "Finished"; }
               }
               if (tmp_How_We_Doing == "Up")
               {
                    if ((tmp_Control_Acc - tmp_Above_Acc) < .01){ tmp_How_We_Doing = "Finished"; }
               }
               
               if (tmp_Timer == 0){ break; }
               tmp_Timer--;
               /**/cout << "  " << tmp_How_We_Doing;
          }
     }
     
     //Autocalibration
     void autocalibration_APT(int p_Timer = -1)
     {
          //double tmp_MC = 0.0f;
          
          //Work through APT.
          
          for (int cou_Con=0;cou_Con<Construct_Count;cou_Con++)
          {
               /**/cout << "\n   Construct->" << cou_Con;
               autocalibrate_Given_Construct_APT(Constructs[cou_Con], settings_APT_Inc, settings_APT_LInc, p_Timer);
          }
          autocalibrate_Given_Construct_APT(&MSC, settings_APT_Inc, settings_APT_LInc, p_Timer);
     }
     
     
     
     //Autocalicrates a given constructs APT
     void autocalibrate_Given_Construct_MC(c_NT3_Construct_1D * p_Con, double p_Inc, double p_LInc, int p_Timer = -1)
     {
          //tmp vars
          double tmp_Control_Acc = 0.0f;
          double tmp_Above_Acc = 0.0f;
          double tmp_Below_Acc = 0.0f;
          bool tmp_Done = false;
          string tmp_How_We_Doing = "Starting";
          double tmp_MC = 0.0f;
          double tmp_MC_Inc = p_Inc;
          int tmp_Timer = p_Timer;
               
          while(!tmp_Done)
          {
               tmp_Done = true;
               
               if (tmp_How_We_Doing == "Starting")
               {
                    //Get the current MC
                    tmp_MC = p_Con->settings_Modifier_Charge;
                    
                    //Gather the control.
                    Eval_Freely();
                    tmp_Control_Acc = gather_Accuracy();
                    
                    //Gather the above.
                    p_Con->settings_Modifier_Charge = (tmp_MC + tmp_MC_Inc);
                    
                    Eval_Freely();
                    tmp_Above_Acc = gather_Accuracy();
                    
                    //Gather below.
                    p_Con->settings_Modifier_Charge = (tmp_MC - tmp_MC_Inc);
                    if ((tmp_MC - tmp_MC_Inc) < 0){ p_Con->settings_Modifier_Charge = 0; }
                    
                    Eval_Freely();
                    tmp_Below_Acc = gather_Accuracy();
                    
                    tmp_Done = false;
                    
                    //Reset the constructs MC so that the next run does not fail.
                    p_Con->settings_Modifier_Charge = tmp_MC;
               }
               if (tmp_How_We_Doing == "Down")
               {
                    //Set the current MC
                    tmp_MC -= tmp_MC_Inc;
                    if (tmp_MC < 0){ tmp_MC = 0; }
                    
                    //The acc_High is not equal to the current low.
                    tmp_Above_Acc = tmp_Control_Acc;
                    
                    //The current control is now equal to the low
                    tmp_Control_Acc = tmp_Below_Acc;
                    
                    //Gather the new low.
                    p_Con->settings_Modifier_Charge = (tmp_MC - tmp_MC_Inc);
                    if ((tmp_MC - tmp_MC_Inc) < 0){ p_Con->settings_Modifier_Charge = 0; }
                    
                    Eval_Freely();
                    tmp_Below_Acc = gather_Accuracy();
                    
                    tmp_Done = false;
                    
                    //Reset the constructs MC so that the next run does not fail.
                    p_Con->settings_Modifier_Charge = tmp_MC;
               }
               if (tmp_How_We_Doing == "Up")
               {
                    //Set the current MC
                    tmp_MC += tmp_MC_Inc;
                    
                    //The acc_High is not equal to the current control.
                    tmp_Below_Acc = tmp_Control_Acc;
                    
                    //Set the control to the current high
                    tmp_Control_Acc = tmp_Above_Acc;
                    
                    //Gather the new high.
                    p_Con->settings_Modifier_Charge = (tmp_MC + tmp_MC_Inc);
                    
                    Eval_Freely();
                    tmp_Above_Acc = gather_Accuracy();
                    
                    tmp_Done = false;
                    
                    //Reset the constructs MC so that the next run does not fail.
                    p_Con->settings_Modifier_Charge = tmp_MC;
               }
               if (tmp_How_We_Doing == "Reduction")
               {
                    tmp_MC_Inc /= 2;
                    if (tmp_MC_Inc < settings_MC_LInc){ break; }
                    
                    tmp_Done = false;
                    tmp_How_We_Doing = "Starting";
                    tmp_MC = p_Con->settings_Modifier_Charge;
                    continue;
               }
               
               
               cout << "\n     A->";
               oflt(0, 15, tmp_Above_Acc);
               cout << " C->";
               oflt(0, 14, tmp_Control_Acc);
               cout << " B->";
               oflt(0, 13, tmp_Below_Acc);
               
               cout << " MC->";
               oflt(0, 12, tmp_MC);
               cout << " MC_I->";
               oflt(0, 11, tmp_MC_Inc);
               
 
               //Reset the constructs MC so that the next run does not fail.
               p_Con->settings_Modifier_Charge = tmp_MC;
               
               //If all results are equal then return.
               if (tmp_Control_Acc == tmp_Above_Acc && tmp_Above_Acc == tmp_Below_Acc){ tmp_Done = true; tmp_How_We_Doing = "Finished"; break; }
               
               //If the control has reached the current best position then reduce the increment and hone in more.
               if (tmp_Control_Acc <= tmp_Above_Acc || tmp_Control_Acc <= tmp_Below_Acc){ tmp_How_We_Doing = "Reduction"; }
               
               //If the one below the control is better move in that direction.
               if (tmp_Control_Acc > tmp_Below_Acc){ tmp_How_We_Doing = "Down"; }
               
               //If the above is better move in that direction, but only if it is also better than below.
               if (tmp_Control_Acc > tmp_Above_Acc && tmp_Above_Acc < tmp_Below_Acc){ tmp_How_We_Doing = "Up"; }
               
               if (tmp_How_We_Doing == "Down")
               {
                    if ((tmp_Control_Acc - tmp_Below_Acc) < .01){ tmp_How_We_Doing = "Finished"; }
               }
               if (tmp_How_We_Doing == "Up")
               {
                    if ((tmp_Control_Acc - tmp_Above_Acc) < .1){ tmp_How_We_Doing = "Finished"; }
               }
               
               if (tmp_Timer == 0){ break; }
               tmp_Timer--;
               /**/cout << "  " << tmp_How_We_Doing << tmp_Timer;
          }
     }
     
     
     //Autocalibration
     void autocalibration_MC(int p_Timer = -1)
     {
          //double tmp_MC = 0.0f;
          
          //Work through APT.
          
          for (int cou_Con=0;cou_Con<Construct_Count;cou_Con++)
          {
               /**/cout << "\n   Construct->" << cou_Con;
               autocalibrate_Given_Construct_MC(Constructs[cou_Con], settings_MC_Inc, settings_MC_LInc, p_Timer);
          }
          autocalibrate_Given_Construct_MC(&MSC, settings_APT_Inc, settings_APT_LInc, p_Timer);
     }
     
     
     
     //Autocalicrates a given constructs BC
     void autocalibrate_Given_Construct_BC(c_NT3_Construct_1D * p_Con, double p_Inc, double p_LInc, int p_Timer = -1)
     {
          //tmp vars
          double tmp_Control_Acc = 0.0f;
          double tmp_Above_Acc = 0.0f;
          double tmp_Below_Acc = 0.0f;
          bool tmp_Done = false;
          string tmp_How_We_Doing = "Starting";
          double tmp_BC = 0.0f;
          double tmp_BC_Inc = p_Inc;
          int tmp_Timer = p_Timer;
          
          while(!tmp_Done)
          {
               tmp_Done = true;
               
               if (tmp_How_We_Doing == "Starting")
               {
                    //Get the current BC
                    tmp_BC = p_Con->settings_Base_Charge;
                    
                    //Gather the control.
                    tmp_Control_Acc = gather_Accuracy();
                    
                    //Gather the above.
                    p_Con->settings_Base_Charge = (tmp_BC + tmp_BC_Inc);
                    
                    Eval_Freely();
                    tmp_Above_Acc = gather_Accuracy();
                    
                    //Gather below.
                    p_Con->settings_Base_Charge = (tmp_BC - tmp_BC_Inc);
                    if ((tmp_BC - tmp_BC_Inc) < 0){ p_Con->settings_Base_Charge = 0; }
                    
                    Eval_Freely();
                    tmp_Below_Acc = gather_Accuracy();
                    
                    tmp_Done = false;
                    
                    //Reset the constructs BC so that the next run does not fail.
                    p_Con->settings_Base_Charge = tmp_BC;
               }
               if (tmp_How_We_Doing == "Down")
               {
                    //Set the current BC
                    tmp_BC -= tmp_BC_Inc;
                    if (tmp_BC < 0){ tmp_BC = 0; }
                    
                    //The acc_High is not equal to the current low.
                    tmp_Above_Acc = tmp_Control_Acc;
                    
                    //The current control is now equal to the low
                    tmp_Control_Acc = tmp_Below_Acc;
                    
                    //Gather the new low.
                    p_Con->settings_Base_Charge = (tmp_BC - tmp_BC_Inc);
                    if ((tmp_BC - tmp_BC_Inc) < 0){ p_Con->settings_Base_Charge = 0; }
                    
                    Eval_Freely();
                    tmp_Below_Acc = gather_Accuracy();
                    
                    tmp_Done = false;
                    
                    //Reset the constructs BC so that the next run does not fail.
                    p_Con->settings_Base_Charge = tmp_BC;
               }
               if (tmp_How_We_Doing == "Up")
               {
                    //Set the current BC
                    tmp_BC += tmp_BC_Inc;
                    
                    //The acc_High is not equal to the current control.
                    tmp_Below_Acc = tmp_Control_Acc;
                    
                    //Set the control to the current high
                    tmp_Control_Acc = tmp_Above_Acc;
                    
                    //Gather the new high.
                    p_Con->settings_Base_Charge = (tmp_BC + tmp_BC_Inc);
                    
                    Eval_Freely();
                    tmp_Above_Acc = gather_Accuracy();
                    
                    tmp_Done = false;
                    
                    //Reset the constructs BC so that the next run does not fail.
                    p_Con->settings_Base_Charge = tmp_BC;
               }
               if (tmp_How_We_Doing == "Reduction")
               {
                    tmp_BC_Inc /= 2;
                    if (tmp_BC_Inc < settings_BC_LInc){ break; }
                    
                    tmp_Done = false;
                    tmp_How_We_Doing = "Starting";
                    tmp_BC = p_Con->settings_Base_Charge;
                    continue;
               }
               
               
               cout << "\n     A->";
               oflt(0, 15, tmp_Above_Acc);
               cout << " C->";
               oflt(0, 14, tmp_Control_Acc);
               cout << " B->";
               oflt(0, 13, tmp_Below_Acc);
               
               cout << " BC->";
               oflt(0, 12, tmp_BC);
               cout << " BC_I->";
               oflt(0, 11, tmp_BC_Inc);
               
               tmp_How_We_Doing = "Finished";
               
               //If all results are equal then return.
               if ((tmp_Control_Acc == tmp_Above_Acc) && (tmp_Above_Acc == tmp_Below_Acc)){ tmp_Done = true; tmp_How_We_Doing = "Finished"; break; }
               
               //If the control has reached the current best position then reduce the increment and hone in more.
               if ((tmp_Control_Acc <= tmp_Above_Acc) || (tmp_Control_Acc <= tmp_Below_Acc)){ tmp_How_We_Doing = "Reduction"; }
               
               //If the one below the control is better move in that direction.
               if (tmp_Control_Acc > tmp_Below_Acc){ tmp_How_We_Doing = "Down"; }
               
               //If the above is better move in that direction, but only if it is also better than below.
               if ((tmp_Control_Acc > tmp_Above_Acc) && (tmp_Above_Acc < tmp_Below_Acc)){ tmp_How_We_Doing = "Up"; }
               
               if (tmp_How_We_Doing == "Down")
               {
                    if ((tmp_Control_Acc - tmp_Below_Acc) < .01){ tmp_How_We_Doing = "Finished"; }
               }
               if (tmp_How_We_Doing == "Up")
               {
                    if ((tmp_Control_Acc - tmp_Above_Acc) < .1){ tmp_How_We_Doing = "Finished"; }
               }
               
               if (tmp_Timer == 0){ break; }
               tmp_Timer--;
               /**/cout << "  " << tmp_How_We_Doing;
               if (tmp_How_We_Doing == "Finished"){ break; }
          }
     }
     
     //Autocalibration
     void autocalibration_BC(int p_Timer = -1)
     {
          //double tmp_MC = 0.0f;
          
          //Work through BC.
          
          for (int cou_Con=0;cou_Con<Construct_Count;cou_Con++)
          {
               /**/cout << "\n   Construct->" << cou_Con;
               autocalibrate_Given_Construct_BC(Constructs[cou_Con], settings_BC_Inc, settings_BC_LInc, p_Timer);
          }
          autocalibrate_Given_Construct_BC(&MSC, settings_BC_Inc, settings_BC_LInc, p_Timer);
     }
     
     
     
     //Autocalicrates a given constructs FT
     void autocalibrate_Given_Construct_FT(c_NT3_Construct_1D * p_Con, double p_Inc, double p_LInc, int p_Timer = -1)
     {
          //tmp vars
          double tmp_Control_Acc = 0.0f;
          double tmp_Above_Acc = 0.0f;
          double tmp_Below_Acc = 0.0f;
          bool tmp_Done = false;
          string tmp_How_We_Doing = "Starting";
          double tmp_FT = 0.0f;
          double tmp_FT_Inc = p_Inc;
          int tmp_Timer = p_Timer;
          
          while(!tmp_Done)
          {
               tmp_Done = true;
               
               if (tmp_How_We_Doing == "Starting")
               {
                    //Get the current FT
                    tmp_FT = p_Con->settings_Tier_Depth;
                    
                    //Gather the control.
                    tmp_Control_Acc = gather_Accuracy();
                    
                    //Gather the above.
                    p_Con->settings_Tier_Depth = (tmp_FT + tmp_FT_Inc);
                    
                    Eval_Freely();
                    tmp_Above_Acc = gather_Accuracy();
                    
                    //Gather below.
                    p_Con->settings_Tier_Depth = (tmp_FT - tmp_FT_Inc);
                    if ((tmp_FT - tmp_FT_Inc) < 0){ p_Con->settings_Tier_Depth = settings_FT_LInc; }
                    
                    Eval_Freely();
                    tmp_Below_Acc = gather_Accuracy();
                    
                    tmp_Done = false;
                    
                    //Reset the constructs FT so that the next run does not fail.
                    p_Con->settings_Tier_Depth = tmp_FT;
               }
               if (tmp_How_We_Doing == "Down")
               {
                    //Set the current FT
                    tmp_FT -= tmp_FT_Inc;
                    if (tmp_FT < 0){ tmp_FT = settings_FT_LInc; }
                    
                    //The acc_High is not equal to the current low.
                    tmp_Above_Acc = tmp_Control_Acc;
                    
                    //The current control is now equal to the low
                    tmp_Control_Acc = tmp_Below_Acc;
                    
                    //Gather the new low.
                    p_Con->settings_Tier_Depth = (tmp_FT - tmp_FT_Inc);
                    if ((tmp_FT - tmp_FT_Inc) < 0){ p_Con->settings_Tier_Depth = 0; }
                    
                    Eval_Freely();
                    tmp_Below_Acc = gather_Accuracy();
                    
                    tmp_Done = false;
                    
                    //Reset the constructs FT so that the next run does not fail.
                    p_Con->settings_Tier_Depth = tmp_FT;
               }
               if (tmp_How_We_Doing == "Up")
               {
                    //Set the current FT
                    tmp_FT += tmp_FT_Inc;
                    
                    //The acc_High is not equal to the current control.
                    tmp_Below_Acc = tmp_Control_Acc;
                    
                    //Set the control to the current high
                    tmp_Control_Acc = tmp_Above_Acc;
                    
                    //Gather the new high.
                    p_Con->settings_Tier_Depth = (tmp_FT + tmp_FT_Inc);
                    
                    Eval_Freely();
                    tmp_Above_Acc = gather_Accuracy();
                    
                    tmp_Done = false;
                    
                    //Reset the constructs FT so that the next run does not fail.
                    p_Con->settings_Tier_Depth = tmp_FT;
               }
               if (tmp_How_We_Doing == "Reduction")
               {
                    tmp_FT_Inc /= 2;
                    if (tmp_FT_Inc < settings_FT_LInc){ break; }
                    
                    tmp_Done = false;
                    tmp_How_We_Doing = "Starting";
                    tmp_FT = p_Con->settings_Tier_Depth;
                    continue;
               }
               
               
               cout << "\n     A->";
               oflt(0, 15, tmp_Above_Acc);
               cout << " C->";
               oflt(0, 14, tmp_Control_Acc);
               cout << " B->";
               oflt(0, 13, tmp_Below_Acc);
               
               cout << " FT->";
               oflt(0, 12, tmp_FT);
               cout << " FT_I->";
               oflt(0, 11, tmp_FT_Inc);
               
               
               //If all results are equal then return.
               if (tmp_Control_Acc == tmp_Above_Acc && tmp_Above_Acc == tmp_Below_Acc){ tmp_Done = true; tmp_How_We_Doing = "Finished"; break; }
               
               //If the control has reached the current best position then reduce the increment and hone in more.
               if (tmp_Control_Acc <= tmp_Above_Acc || tmp_Control_Acc <= tmp_Below_Acc){ tmp_How_We_Doing = "Reduction"; }
               
               //If the one below the control is better move in that direction.
               if (tmp_Control_Acc > tmp_Below_Acc){ tmp_How_We_Doing = "Down"; }
               
               //If the above is better move in that direction, but only if it is also better than below.
               if (tmp_Control_Acc > tmp_Above_Acc && tmp_Above_Acc < tmp_Below_Acc){ tmp_How_We_Doing = "Up"; }
               
               if (tmp_Timer == 0){ break; }
               tmp_Timer--;
               /**/cout << "  " << tmp_How_We_Doing;
          }
     }
     
     //Autocalibration
     void autocalibration_FT(int p_Timer = -1)
     {
          //double tmp_MC = 0.0f;
          
          //Work through FT.
          
          for (int cou_Con=0;cou_Con<Construct_Count;cou_Con++)
          {
               /**/cout << "\n   Construct->" << cou_Con;
               autocalibrate_Given_Construct_FT(Constructs[cou_Con], settings_FT_Inc, settings_FT_LInc, p_Timer);
          }
          autocalibrate_Given_Construct_FT(&MSC, settings_FT_Inc, settings_FT_LInc, p_Timer);
     }
     
     
     
     //Autocalicrates a given constructs FT
     void autocalibrate_Given_Construct_RC(c_NT3_Construct_1D * p_Con)
     {
          //tmp vars
          double tmp_NO_RC_Acc = 0.0f;
          double tmp_RC_Acc = 0.0f;
          
          
          //Gather the control.
          p_Con->sauto_Use_RC = 0;
          Eval();
          tmp_NO_RC_Acc = gather_Accuracy();
          
          //Gather the control.
          p_Con->sauto_Use_RC = 1;
          Eval();
          tmp_RC_Acc = gather_Accuracy();
          
          cout << "\n     E->";
          oflt(0, 15, tmp_NO_RC_Acc);
          cout << " ERC->";
          oflt(0, 14, tmp_RC_Acc);
          
          //If the one below the control is better move in that direction.
          if (tmp_NO_RC_Acc > tmp_RC_Acc){ p_Con->sauto_Use_RC = 1; }
          if (tmp_RC_Acc > tmp_NO_RC_Acc){ p_Con->sauto_Use_RC = 0; }
          
     }
     
     //Autocalibration
     void autocalibration_RC(int p_Timer = -1)
     {
          //double tmp_MC = 0.0f;
          
          //Work through FT.
          
          for (int cou_Con=0;cou_Con<Construct_Count;cou_Con++)
          {
               /**/cout << "\n   Construct->" << cou_Con;
               autocalibrate_Given_Construct_RC(Constructs[cou_Con]);
          }
          autocalibrate_Given_Construct_RC(&MSC);
     }
     
     
     //==-- AUTOCALIBRATE USING THE VISION ACCURACY
     
     
     //Autocalicrates a given constructs APT
     void vautocalibrate_Given_Construct_APT(c_NT3_Construct_1D * p_Con, double p_Inc, double p_LInc)
     {
          //tmp vars
          double tmp_Control_Acc = 0.0f;
          double tmp_Above_Acc = 0.0f;
          double tmp_Below_Acc = 0.0f;
          bool tmp_Done = false;
          string tmp_How_We_Doing = "Starting";
          double tmp_APT = 0.0f;
          double tmp_APT_Inc = p_Inc;
          
          while(!tmp_Done)
          {
               tmp_Done = true;
               
               if (tmp_How_We_Doing == "Starting")
               {
                    //Get the current APT
                    tmp_APT = p_Con->settings_Action_Potential_Threshold;
                    
                    //Gather the control.
                    tmp_Control_Acc = gather_VAccuracy();
                    
                    //Gather the above.
                    p_Con->settings_Action_Potential_Threshold = (tmp_APT + tmp_APT_Inc);
                    
                    Eval_Freely();
                    tmp_Above_Acc = gather_VAccuracy();
                    
                    //Gather below.
                    p_Con->settings_Action_Potential_Threshold = (tmp_APT - tmp_APT_Inc);
                    if ((tmp_APT - tmp_APT_Inc) < 0){ p_Con->settings_Action_Potential_Threshold = 0; }
                    
                    Eval_Freely();
                    tmp_Below_Acc = gather_VAccuracy();
                    
                    tmp_Done = false;
                    
                    //Reset the constructs APT so that the next run does not fail.
                    p_Con->settings_Action_Potential_Threshold = tmp_APT;
               }
               if (tmp_How_We_Doing == "Down")
               {
                    //Set the current APT
                    tmp_APT -= tmp_APT_Inc;
                    if (tmp_APT < 0){ tmp_APT = 0; }
                    
                    //The acc_High is not equal to the current low.
                    tmp_Above_Acc = tmp_Control_Acc;
                    
                    //The current control is now equal to the low
                    tmp_Control_Acc = tmp_Below_Acc;
                    
                    //Gather the new low.
                    p_Con->settings_Action_Potential_Threshold = (tmp_APT - tmp_APT_Inc);
                    if ((tmp_APT - tmp_APT_Inc) < 0){ p_Con->settings_Action_Potential_Threshold = 0; }
                    
                    Eval_Freely();
                    tmp_Below_Acc = gather_VAccuracy();
                    
                    tmp_Done = false;
                    
                    //Reset the constructs APT so that the next run does not fail.
                    p_Con->settings_Action_Potential_Threshold = tmp_APT;
               }
               if (tmp_How_We_Doing == "Up")
               {
                    //Set the current APT
                    tmp_APT += tmp_APT_Inc;
                    
                    //The acc_High is not equal to the current control.
                    tmp_Below_Acc = tmp_Control_Acc;
                    
                    //Set the control to the current high
                    tmp_Control_Acc = tmp_Above_Acc;
                    
                    //Gather the new high.
                    p_Con->settings_Action_Potential_Threshold = (tmp_APT + tmp_APT_Inc);
                    
                    Eval_Freely();
                    tmp_Above_Acc = gather_VAccuracy();
                    
                    tmp_Done = false;
                    
                    //Reset the constructs APT so that the next run does not fail.
                    p_Con->settings_Action_Potential_Threshold = tmp_APT;
               }
               if (tmp_How_We_Doing == "Reduction")
               {
                    tmp_APT_Inc /= 2;
                    if (tmp_APT_Inc < settings_APT_LInc){ break; }
                    
                    tmp_Done = false;
                    tmp_How_We_Doing = "Starting";
                    tmp_APT = p_Con->settings_Action_Potential_Threshold;
                    continue;
               }
               
               
               cout << "\n     A->";
               oflt(0, 15, tmp_Above_Acc);
               cout << " C->";
               oflt(0, 14, tmp_Control_Acc);
               cout << " B->";
               oflt(0, 13, tmp_Below_Acc);
               
               cout << " APT->";
               oflt(0, 12, tmp_APT);
               cout << " APT_I->";
               oflt(0, 11, tmp_APT_Inc);
               
               
               //If all results are equal then return.
               if (tmp_Control_Acc == tmp_Above_Acc && tmp_Above_Acc == tmp_Below_Acc){ tmp_Done = true; tmp_How_We_Doing = "Finished"; break; }
               
               //If the control has reached the current best position then reduce the increment and hone in more.
               if (tmp_Control_Acc <= tmp_Above_Acc || tmp_Control_Acc <= tmp_Below_Acc){ tmp_How_We_Doing = "Reduction"; }
               
               //If the one below the control is better move in that direction.
               if (tmp_Control_Acc > tmp_Below_Acc){ tmp_How_We_Doing = "Down"; }
               
               //If the above is better move in that direction, but only if it is also better than below.
               if (tmp_Control_Acc > tmp_Above_Acc && tmp_Above_Acc < tmp_Below_Acc){ tmp_How_We_Doing = "Up"; }
               
               /**/cout << "  " << tmp_How_We_Doing;
          }
     }
     
     //Autocalibration
     void vautocalibration_APT()
     {
          //double tmp_MC = 0.0f;
          
          //Work through APT.
          
          for (int cou_Con=0;cou_Con<Construct_Count;cou_Con++)
          {
               /**/cout << "\n   Construct->" << cou_Con;
               vautocalibrate_Given_Construct_APT(Constructs[cou_Con], settings_APT_Inc, settings_APT_LInc);
          }
          vautocalibrate_Given_Construct_APT(&MSC, settings_APT_Inc, settings_APT_LInc);
     }
     
     
     
     //Autocalicrates a given constructs APT
     void vautocalibrate_Given_Construct_MC(c_NT3_Construct_1D * p_Con, double p_Inc, double p_LInc)
     {
          //tmp vars
          double tmp_Control_Acc = 0.0f;
          double tmp_Above_Acc = 0.0f;
          double tmp_Below_Acc = 0.0f;
          bool tmp_Done = false;
          string tmp_How_We_Doing = "Starting";
          double tmp_MC = 0.0f;
          double tmp_MC_Inc = p_Inc;
               
          while(!tmp_Done)
          {
               tmp_Done = true;
               
               if (tmp_How_We_Doing == "Starting")
               {
                    //Get the current MC
                    tmp_MC = p_Con->settings_Modifier_Charge;
                    
                    //Gather the control.
                    Eval_Freely();
                    tmp_Control_Acc = gather_VAccuracy();
                    
                    //Gather the above.
                    p_Con->settings_Modifier_Charge = (tmp_MC + tmp_MC_Inc);
                    
                    Eval_Freely();
                    tmp_Above_Acc = gather_VAccuracy();
                    
                    //Gather below.
                    p_Con->settings_Modifier_Charge = (tmp_MC - tmp_MC_Inc);
                    if ((tmp_MC - tmp_MC_Inc) < 0){ p_Con->settings_Modifier_Charge = 0; }
                    
                    Eval_Freely();
                    tmp_Below_Acc = gather_VAccuracy();
                    
                    tmp_Done = false;
                    
                    //Reset the constructs MC so that the next run does not fail.
                    p_Con->settings_Modifier_Charge = tmp_MC;
               }
               if (tmp_How_We_Doing == "Down")
               {
                    //Set the current MC
                    tmp_MC -= tmp_MC_Inc;
                    if (tmp_MC < 0){ tmp_MC = 0; }
                    
                    //The acc_High is not equal to the current low.
                    tmp_Above_Acc = tmp_Control_Acc;
                    
                    //The current control is now equal to the low
                    tmp_Control_Acc = tmp_Below_Acc;
                    
                    //Gather the new low.
                    p_Con->settings_Modifier_Charge = (tmp_MC - tmp_MC_Inc);
                    if ((tmp_MC - tmp_MC_Inc) < 0){ p_Con->settings_Modifier_Charge = 0; }
                    
                    Eval_Freely();
                    tmp_Below_Acc = gather_VAccuracy();
                    
                    tmp_Done = false;
                    
                    //Reset the constructs MC so that the next run does not fail.
                    p_Con->settings_Modifier_Charge = tmp_MC;
               }
               if (tmp_How_We_Doing == "Up")
               {
                    //Set the current MC
                    tmp_MC += tmp_MC_Inc;
                    
                    //The acc_High is not equal to the current control.
                    tmp_Below_Acc = tmp_Control_Acc;
                    
                    //Set the control to the current high
                    tmp_Control_Acc = tmp_Above_Acc;
                    
                    //Gather the new high.
                    p_Con->settings_Modifier_Charge = (tmp_MC + tmp_MC_Inc);
                    
                    Eval_Freely();
                    tmp_Above_Acc = gather_VAccuracy();
                    
                    tmp_Done = false;
                    
                    //Reset the constructs MC so that the next run does not fail.
                    p_Con->settings_Modifier_Charge = tmp_MC;
               }
               if (tmp_How_We_Doing == "Reduction")
               {
                    tmp_MC_Inc /= 2;
                    if (tmp_MC_Inc < settings_MC_LInc){ break; }
                    
                    tmp_Done = false;
                    tmp_How_We_Doing = "Starting";
                    tmp_MC = p_Con->settings_Modifier_Charge;
                    continue;
               }
               
               
               cout << "\n     A->";
               oflt(0, 15, tmp_Above_Acc);
               cout << " C->";
               oflt(0, 14, tmp_Control_Acc);
               cout << " B->";
               oflt(0, 13, tmp_Below_Acc);
               
               cout << " MC->";
               oflt(0, 12, tmp_MC);
               cout << " MC_I->";
               oflt(0, 11, tmp_MC_Inc);
               
 
               //Reset the constructs MC so that the next run does not fail.
               p_Con->settings_Modifier_Charge = tmp_MC;
               
               //If all results are equal then return.
               if (tmp_Control_Acc == tmp_Above_Acc && tmp_Above_Acc == tmp_Below_Acc){ tmp_Done = true; tmp_How_We_Doing = "Finished"; break; }
               
               //If the control has reached the current best position then reduce the increment and hone in more.
               if (tmp_Control_Acc <= tmp_Above_Acc || tmp_Control_Acc <= tmp_Below_Acc){ tmp_How_We_Doing = "Reduction"; }
               
               //If the one below the control is better move in that direction.
               if (tmp_Control_Acc > tmp_Below_Acc){ tmp_How_We_Doing = "Down"; }
               
               //If the above is better move in that direction, but only if it is also better than below.
               if (tmp_Control_Acc > tmp_Above_Acc && tmp_Above_Acc < tmp_Below_Acc){ tmp_How_We_Doing = "Up"; }
               
               /**/cout << "  " << tmp_How_We_Doing;
          }
     }
     
     
     //Autocalibration
     void vautocalibration_MC()
     {
          //double tmp_MC = 0.0f;
          
          //Work through APT.
          
          for (int cou_Con=0;cou_Con<Construct_Count;cou_Con++)
          {
               /**/cout << "\n   Construct->" << cou_Con;
               vautocalibrate_Given_Construct_MC(Constructs[cou_Con], settings_MC_Inc, settings_MC_LInc);
          }
          vautocalibrate_Given_Construct_MC(&MSC, settings_APT_Inc, settings_APT_LInc);
     }
     
     
     
     //Autocalicrates a given constructs BC
     void vautocalibrate_Given_Construct_BC(c_NT3_Construct_1D * p_Con, double p_Inc, double p_LInc)
     {
          //tmp vars
          double tmp_Control_Acc = 0.0f;
          double tmp_Above_Acc = 0.0f;
          double tmp_Below_Acc = 0.0f;
          bool tmp_Done = false;
          string tmp_How_We_Doing = "Starting";
          double tmp_BC = 0.0f;
          double tmp_BC_Inc = p_Inc;
          
          while(!tmp_Done)
          {
               tmp_Done = true;
               
               if (tmp_How_We_Doing == "Starting")
               {
                    //Get the current BC
                    tmp_BC = p_Con->settings_Base_Charge;
                    
                    //Gather the control.
                    tmp_Control_Acc = gather_VAccuracy();
                    
                    //Gather the above.
                    p_Con->settings_Base_Charge = (tmp_BC + tmp_BC_Inc);
                    
                    Eval_Freely();
                    tmp_Above_Acc = gather_VAccuracy();
                    
                    //Gather below.
                    p_Con->settings_Base_Charge = (tmp_BC - tmp_BC_Inc);
                    if ((tmp_BC - tmp_BC_Inc) < 0){ p_Con->settings_Base_Charge = 0; }
                    
                    Eval_Freely();
                    tmp_Below_Acc = gather_VAccuracy();
                    
                    tmp_Done = false;
                    
                    //Reset the constructs BC so that the next run does not fail.
                    p_Con->settings_Base_Charge = tmp_BC;
               }
               if (tmp_How_We_Doing == "Down")
               {
                    //Set the current BC
                    tmp_BC -= tmp_BC_Inc;
                    if (tmp_BC < 0){ tmp_BC = 0; }
                    
                    //The acc_High is not equal to the current low.
                    tmp_Above_Acc = tmp_Control_Acc;
                    
                    //The current control is now equal to the low
                    tmp_Control_Acc = tmp_Below_Acc;
                    
                    //Gather the new low.
                    p_Con->settings_Base_Charge = (tmp_BC - tmp_BC_Inc);
                    if ((tmp_BC - tmp_BC_Inc) < 0){ p_Con->settings_Base_Charge = 0; }
                    
                    Eval_Freely();
                    tmp_Below_Acc = gather_VAccuracy();
                    
                    tmp_Done = false;
                    
                    //Reset the constructs BC so that the next run does not fail.
                    p_Con->settings_Base_Charge = tmp_BC;
               }
               if (tmp_How_We_Doing == "Up")
               {
                    //Set the current BC
                    tmp_BC += tmp_BC_Inc;
                    
                    //The acc_High is not equal to the current control.
                    tmp_Below_Acc = tmp_Control_Acc;
                    
                    //Set the control to the current high
                    tmp_Control_Acc = tmp_Above_Acc;
                    
                    //Gather the new high.
                    p_Con->settings_Base_Charge = (tmp_BC + tmp_BC_Inc);
                    
                    Eval_Freely();
                    tmp_Above_Acc = gather_VAccuracy();
                    
                    tmp_Done = false;
                    
                    //Reset the constructs BC so that the next run does not fail.
                    p_Con->settings_Base_Charge = tmp_BC;
               }
               if (tmp_How_We_Doing == "Reduction")
               {
                    tmp_BC_Inc /= 2;
                    if (tmp_BC_Inc < settings_BC_LInc){ break; }
                    
                    tmp_Done = false;
                    tmp_How_We_Doing = "Starting";
                    tmp_BC = p_Con->settings_Base_Charge;
                    continue;
               }
               
               
               cout << "\n     A->";
               oflt(0, 15, tmp_Above_Acc);
               cout << " C->";
               oflt(0, 14, tmp_Control_Acc);
               cout << " B->";
               oflt(0, 13, tmp_Below_Acc);
               
               cout << " BC->";
               oflt(0, 12, tmp_BC);
               cout << " BC_I->";
               oflt(0, 11, tmp_BC_Inc);
               
               
               //If all results are equal then return.
               if (tmp_Control_Acc == tmp_Above_Acc && tmp_Above_Acc == tmp_Below_Acc){ tmp_Done = true; tmp_How_We_Doing = "Finished"; break; }
               
               //If the control has reached the current best position then reduce the increment and hone in more.
               if (tmp_Control_Acc <= tmp_Above_Acc || tmp_Control_Acc <= tmp_Below_Acc){ tmp_How_We_Doing = "Reduction"; }
               
               //If the one below the control is better move in that direction.
               if (tmp_Control_Acc > tmp_Below_Acc){ tmp_How_We_Doing = "Down"; }
               
               //If the above is better move in that direction, but only if it is also better than below.
               if (tmp_Control_Acc > tmp_Above_Acc && tmp_Above_Acc < tmp_Below_Acc){ tmp_How_We_Doing = "Up"; }
               
               /**/cout << "  " << tmp_How_We_Doing;
          }
     }
     
     //Autocalibration
     void vautocalibration_BC()
     {
          //double tmp_MC = 0.0f;
          
          //Work through BC.
          
          for (int cou_Con=0;cou_Con<Construct_Count;cou_Con++)
          {
               /**/cout << "\n   Construct->" << cou_Con;
               vautocalibrate_Given_Construct_BC(Constructs[cou_Con], settings_BC_Inc, settings_BC_LInc);
          }
          vautocalibrate_Given_Construct_BC(&MSC, settings_BC_Inc, settings_BC_LInc);
     }
     
     
     
     //Autocalicrates a given constructs FT
     void vautocalibrate_Given_Construct_FT(c_NT3_Construct_1D * p_Con, double p_Inc, double p_LInc)
     {
          //tmp vars
          double tmp_Control_Acc = 0.0f;
          double tmp_Above_Acc = 0.0f;
          double tmp_Below_Acc = 0.0f;
          bool tmp_Done = false;
          string tmp_How_We_Doing = "Starting";
          double tmp_FT = 0.0f;
          double tmp_FT_Inc = p_Inc;
          
          while(!tmp_Done)
          {
               tmp_Done = true;
               
               if (tmp_How_We_Doing == "Starting")
               {
                    //Get the current FT
                    tmp_FT = p_Con->settings_Tier_Depth;
                    
                    //Gather the control.
                    tmp_Control_Acc = gather_VAccuracy();
                    
                    //Gather the above.
                    p_Con->settings_Tier_Depth = (tmp_FT + tmp_FT_Inc);
                    
                    Eval_Freely();
                    tmp_Above_Acc = gather_VAccuracy();
                    
                    //Gather below.
                    p_Con->settings_Tier_Depth = (tmp_FT - tmp_FT_Inc);
                    if ((tmp_FT - tmp_FT_Inc) < 0){ p_Con->settings_Tier_Depth = settings_FT_LInc; }
                    
                    Eval_Freely();
                    tmp_Below_Acc = gather_VAccuracy();
                    
                    tmp_Done = false;
                    
                    //Reset the constructs FT so that the next run does not fail.
                    p_Con->settings_Tier_Depth = tmp_FT;
               }
               if (tmp_How_We_Doing == "Down")
               {
                    //Set the current FT
                    tmp_FT -= tmp_FT_Inc;
                    if (tmp_FT < 0){ tmp_FT = settings_FT_LInc; }
                    
                    //The acc_High is not equal to the current low.
                    tmp_Above_Acc = tmp_Control_Acc;
                    
                    //The current control is now equal to the low
                    tmp_Control_Acc = tmp_Below_Acc;
                    
                    //Gather the new low.
                    p_Con->settings_Tier_Depth = (tmp_FT - tmp_FT_Inc);
                    if ((tmp_FT - tmp_FT_Inc) < 0){ p_Con->settings_Tier_Depth = 0; }
                    
                    Eval_Freely();
                    tmp_Below_Acc = gather_VAccuracy();
                    
                    tmp_Done = false;
                    
                    //Reset the constructs FT so that the next run does not fail.
                    p_Con->settings_Tier_Depth = tmp_FT;
               }
               if (tmp_How_We_Doing == "Up")
               {
                    //Set the current FT
                    tmp_FT += tmp_FT_Inc;
                    
                    //The acc_High is not equal to the current control.
                    tmp_Below_Acc = tmp_Control_Acc;
                    
                    //Set the control to the current high
                    tmp_Control_Acc = tmp_Above_Acc;
                    
                    //Gather the new high.
                    p_Con->settings_Tier_Depth = (tmp_FT + tmp_FT_Inc);
                    
                    Eval_Freely();
                    tmp_Above_Acc = gather_VAccuracy();
                    
                    tmp_Done = false;
                    
                    //Reset the constructs FT so that the next run does not fail.
                    p_Con->settings_Tier_Depth = tmp_FT;
               }
               if (tmp_How_We_Doing == "Reduction")
               {
                    tmp_FT_Inc /= 2;
                    if (tmp_FT_Inc < settings_FT_LInc){ break; }
                    
                    tmp_Done = false;
                    tmp_How_We_Doing = "Starting";
                    tmp_FT = p_Con->settings_Tier_Depth;
                    continue;
               }
               
               
               cout << "\n     A->";
               oflt(0, 15, tmp_Above_Acc);
               cout << " C->";
               oflt(0, 14, tmp_Control_Acc);
               cout << " B->";
               oflt(0, 13, tmp_Below_Acc);
               
               cout << " FT->";
               oflt(0, 12, tmp_FT);
               cout << " FT_I->";
               oflt(0, 11, tmp_FT_Inc);
               
               
               //If all results are equal then return.
               if (tmp_Control_Acc == tmp_Above_Acc && tmp_Above_Acc == tmp_Below_Acc){ tmp_Done = true; tmp_How_We_Doing = "Finished"; break; }
               
               //If the control has reached the current best position then reduce the increment and hone in more.
               if (tmp_Control_Acc <= tmp_Above_Acc || tmp_Control_Acc <= tmp_Below_Acc){ tmp_How_We_Doing = "Reduction"; }
               
               //If the one below the control is better move in that direction.
               if (tmp_Control_Acc > tmp_Below_Acc){ tmp_How_We_Doing = "Down"; }
               
               //If the above is better move in that direction, but only if it is also better than below.
               if (tmp_Control_Acc > tmp_Above_Acc && tmp_Above_Acc < tmp_Below_Acc){ tmp_How_We_Doing = "Up"; }
               
               /**/cout << "  " << tmp_How_We_Doing;
          }
     }
     
     //Autocalibration
     void vautocalibration_FT()
     {
          //double tmp_MC = 0.0f;
          
          //Work through FT.
          
          for (int cou_Con=0;cou_Con<Construct_Count;cou_Con++)
          {
               /**/cout << "\n   Construct->" << cou_Con;
               vautocalibrate_Given_Construct_FT(Constructs[cou_Con], settings_FT_Inc, settings_FT_LInc);
          }
          vautocalibrate_Given_Construct_FT(&MSC, settings_FT_Inc, settings_FT_LInc);
     }
     
     
     
     //Autocalicrates a given constructs FT
     void vautocalibrate_Given_Construct_RC(c_NT3_Construct_1D * p_Con)
     {
          //tmp vars
          double tmp_NO_RC_Acc = 0.0f;
          double tmp_RC_Acc = 0.0f;
          
          Eval();
          
          //Gather the control.
          tmp_NO_RC_Acc = gather_VAccuracy();
          
          //Gather the control.
          tmp_RC_Acc = gather_VAccuracy();
          
          cout << "\n     E->";
          oflt(0, 15, tmp_NO_RC_Acc);
          cout << " ERC->";
          oflt(0, 14, tmp_RC_Acc);
          
          //If the one below the control is better move in that direction.
          if (tmp_NO_RC_Acc > tmp_RC_Acc){ p_Con->sauto_Use_RC = 1; }
          if (tmp_RC_Acc > tmp_NO_RC_Acc){ p_Con->sauto_Use_RC = 0; }
          
     }
     
     //Autocalibration
     void vautocalibration_RC()
     {
          //double tmp_MC = 0.0f;
          
          //Work through FT.
          
          for (int cou_Con=0;cou_Con<Construct_Count;cou_Con++)
          {
               /**/cout << "\n   Construct->" << cou_Con;
               vautocalibrate_Given_Construct_RC(Constructs[cou_Con]);
          }
          vautocalibrate_Given_Construct_RC(&MSC);
     }
     
     
     ////==---------------------+
     //==--   OUTPUT FUNCTIONS
     ////==---------------------+
     
     //==--  Console Outputs
     
     //Outputs the input tables. 
     void output_Assembly_Inputs(int p_Display = 0)
     {  
          cout << "\n\n --==   Assembly Input Tables  ==--";
          tbl_Input.output_C();
          tbl_Input.output_F();
          tbl_Input.output_I();
          tbl_Input_Goal.output_C();
          tbl_Input_Goal.output_F();
          tbl_Input_Goal.output_I();
          tbl_Assembly_Treetops.output_I();
          
     } 
     
     //Outputs the input tables. 
     void output_Input_Tables(int p_Display = 0)
     {  
          cout << "\n\n --==   Assembly Input Tables  ==--";
          tbl_Input.output_C();
          tbl_Input.output_F();
          tbl_Input.output_I();
          tbl_Input_Goal.output_C();
          tbl_Input_Goal.output_F();
          tbl_Input_Goal.output_I();
          tbl_Assembly_Treetops.output_I();
          
          cout << "\n\n --==   MSC Input Tables  ==--";
          if (p_Display == 0){ MSC.output_Input_Table(); }
          if (p_Display == 1){ MSC.output_Input_Table_I(); }
          if (p_Display == 2){ MSC.output_Input_Table_F(); }
          if (p_Display == 3){ MSC.output_Input_Table_NR(); }
          
          cout << "\n\n --==   Construct Input Tables  ==--";
          for (int cou_C=0;cou_C<Construct_Count;cou_C++)
          {
               if (p_Display == 0){ Constructs[cou_C]->output_Input_Table(); }
               if (p_Display == 1){ Constructs[cou_C]->output_Input_Table_I(); }
               if (p_Display == 2){ Constructs[cou_C]->output_Input_Table_F(); }
               if (p_Display == 3){ Constructs[cou_C]->output_Input_Table_NR(); }
          }
     } 
     
     //Outputs the output tables. 
     void output_Output_Tables()
     {  
          cout << "\n\n --==   Assembly Output Tables  ==--";
          
          cout << "\n\n --==   Construct Output Tables  ==--";
          MSC.output_Output_Tables();
          
          cout << "\n\n --==   Construct Output Tables  ==--";
          for (int cou_C=0;cou_C<Construct_Count;cou_C++)
          {
               Constructs[cou_C]->output_Output_Tables();
               Constructs[cou_C]->output_Output_Tables_I();
               Constructs[cou_C]->output_Output_Tables_F();
          }
     } 
     
     //Outputs the output tables. 
     void output_Output_Tables_I()
     {  
          cout << "\n\n --==   Assembly Output Tables  ==--";
          
          cout << "\n\n --==   Construct Output Tables  ==--";
          MSC.output_Output_Tables_I();
          
          cout << "\n\n --==   Construct Output Tables  ==--";
          for (int cou_C=0;cou_C<Construct_Count;cou_C++)
          {
               Constructs[cou_C]->output_Output_Tables_I();
          }
     } 
     
     //Outputs the output tables for a given construct.
     void output_Construct_Output_Tables(int p_Construct)
     {
          if (p_Construct >= Construct_Count){ return; }
          
          Constructs[p_Construct]->output_Output_Tables();
     }
     
     //Outputs the output tables for a given construct.
     void output_Construct_Output_Tables_F(int p_Construct)
     {
          if (p_Construct >= Construct_Count){ return; }
          
          Constructs[p_Construct]->output_Output_Tables_F();
     }
      
     //Outputs the treetop tables.
     void output_Treetop_Tables()
     {
          MSC.tbl_Treetops.output_I();
          for (int cou_C=0;cou_C<Construct_Count;cou_C++)
          {
               Constructs[cou_C]->tbl_Treetops.output_I();
          }
     }
     
     
     //Outputs all of the nodes in the node networks.
     void output_Nodes()
     {
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               Constructs[cou_Index]->output_Nodes();
          }
          MSC.output_Nodes();
     }
     
     //Outputs all of the CANs
     void output_CAN_States()
     {
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               Constructs[cou_Index]->output_CAN_State();
          }
          MSC.output_CAN_State();
     }
     
     //Outputs the current settings.
     void output_Settings()
     {
          cout << "\n\n MSC "; ostr(0, 13, Name); cout << " Settings...";
          cout << " settings_Compress_MSC_Outputs->"; oint(0, 8, settings_Compress_MSC_Outputs);
          cout << " Action_Potential_Threshold->"; odbl(0, 8, Action_Potential_Threshold);
          cout << " Base_Charge->"; oint(0, 8, Base_Charge);
          
          MSC.output_Settings();
          
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               Constructs[cou_Index]->output_Settings();
          }
     }
     
     void output_Settings_Full()
     {
          cout << "\n\n MSC "; ostr(0, 13, Name); cout << " Settings...";
          cout << " settings_Compress_MSC_Outputs->"; oint(0, 8, settings_Compress_MSC_Outputs);
          cout << " Action_Potential_Threshold->"; odbl(0, 8, Action_Potential_Threshold);
          cout << " Base_Charge->"; oint(0, 8, Base_Charge);
          
          MSC.output_Settings_Full();
          
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               Constructs[cou_Index]->output_Settings_Full();
          }
     }
     
     ////==---------------------------------+
     //==--   SAVING AND LOADING FUNCTIONS
     ////==---------------------------------+
     
     //Saves the construct.
     void Save(string p_Name, string p_Dir = "NT3.Save")
     {
          cout << "\n\n\t Saving MSCA " << Name << "..........";
          
          //Create the save file and directory.
          ofstream SF;
          string tmp_Dir = p_Dir + "\\" + p_Name + "\\";
          string tmp_FName = tmp_Dir + "Assembly.dat";
          string tmp_Construct_Name = "";
          
          //Create the directory using the CMD commands.
          string tmp_MKDIR = "mkdir " + tmp_Dir;
          cout << "\n\n\n " << tmp_MKDIR;
          system(tmp_MKDIR.c_str());
          
          
          SF.open(tmp_FName, ios::trunc);
          
          
          
          if (!SF.is_open()){ cout << "ACCESS DENIED"; return; } else { cout << "ACCESS GRANTED"; }
          
          //--  Save settings
          
          cout << "\n\t Output File Secure, Saving Settings..........";
          
          SF << Name;
          SF << "\n" << Construct_Count;
          
          //Save the MSC.
          MSC.Save("MSC", tmp_Dir);
          
          //Save the construct names in the asb linker file.
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               tmp_Construct_Name = Constructs[cou_Index]->Name + "." + int2str(cou_Index);
               SF << "\n" << tmp_Construct_Name;
          }
          SF.close();
          
          //Save the constructs.
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               tmp_Construct_Name = Constructs[cou_Index]->Name + "." + int2str(cou_Index);
               Constructs[cou_Index]->Save(tmp_Construct_Name, tmp_Dir);
          }
     } 
     
     //Loads a construct.     
     void Load(string p_Name, string p_Dir = "NT3.Save")
     {
          cout << "\n\n\t Loading MSCA " << p_Dir << "/" << p_Name << "..........";
          
          //Create the save file and directory.
          ifstream SF;
          string tmp_Dir = p_Dir + "\\" + p_Name + "\\";
          string tmp_FName = tmp_Dir + "Assembly.dat";
          string tmp_Construct_Name = "";
          int tmp_Construct_Count = 0;
          
          SF.open(tmp_FName);
          
          if (!SF.is_open()){ cout << "ACCESS DENIED"; return; } else { cout << "ACCESS GRANTED"; }
          
          //--  Save settings
          
          cout << "\n\t Input File Secure, Loading Settings..........";
          
          SF >> Name;
          set_Name(Name);
          
          SF >> tmp_Construct_Count;
          tbl_Input.resize(tmp_Construct_Count);
          tbl_Input_Goal.resize(tmp_Construct_Count);
          resize(tmp_Construct_Count);
          
          cout << "DONE";
          
          
          //Load the MSC.
          MSC.Load("MSC", tmp_Dir);
          
          //Save the construct names in the asb linker file.
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               SF >> tmp_Construct_Name;
               Constructs[cou_Index]->Load(tmp_Construct_Name, tmp_Dir);
          }
          SF.close();
     } 
     
     
     
     
     ////==--------------+
     //==--   INTERFACE
     ////==--------------+
     
     //Accepts an input to the inputs motherfucker.
     void in(int p_Row, int p_Cell, string p_Submission)
     {
          tbl_Input.set_String(p_Row, p_Cell, p_Submission);
     }
     
     //Accepts an input to the inputs motherfucker.
     void in(int p_Row, int p_Cell, int p_Submission)
     {
          tbl_Input.set_Int(p_Row, p_Cell, p_Submission);
     }
     
     //Accepts an input to the inputs motherfucker.
     void in(int p_Row, int p_Cell, double p_Submission)
     {
          tbl_Input.set_Float(p_Row, p_Cell, p_Submission);
     }
     
     //Accepts an input to the inputs motherfucker.
     void in(int p_Row, int p_Cell, void * p_Submission)
     {
          tbl_Input.set_NR(p_Row, p_Cell, p_Submission);
     }
     
     
     //==--   Adds the submission to the given cell.
     //Accepts an input to the inputs motherfucker.
     void ina(int p_Row, int p_Cell, int p_Submission)
     {
          tbl_Input.add_Data_Int(p_Row, p_Cell, p_Submission);
     }
     
     //Accepts an input to the inputs motherfucker.
     void ina(int p_Row, int p_Cell, double p_Submission)
     {
          tbl_Input.add_Data_Float(p_Row, p_Cell, p_Submission);
     }
     
     //Accepts an input to the inputs motherfucker.
     void ina(int p_Row, int p_Cell, void * p_Submission)
     {
          tbl_Input.add_Data_NR(p_Row, p_Cell, p_Submission);
     }
     
     
     
     
     //Accepts an array to be input into a row.
     void ina_A(int p_Row, int p_Cell, int * p_Submission, int p_Length)
     {
          for (int cou_Index=0;cou_Index<p_Length;cou_Index++)
          {
               ina(p_Row, p_Cell, p_Submission[cou_Index]);
          }
     }
     
     //Accepts an array to be input into a row.
     void ina_A(int p_Row, int p_Cell, double * p_Submission, int p_Length)
     {
          for (int cou_Index=0;cou_Index<p_Length;cou_Index++)
          {
               ina(p_Row, p_Cell, p_Submission[cou_Index]);
          }
     }
     
     //Accepts an array to be input into a row.
     void ina_A(int p_Row, int p_Cell, void ** p_Submission, int p_Length)
     {
          for (int cou_Index=0;cou_Index<p_Length;cou_Index++)
          {
               ina(p_Row, p_Cell, p_Submission[cou_Index]);
          }
     }
     
     //Accepts an array to be input into a row.
     void in_A(int p_Row, int p_Cell, int * p_Submission, int p_Length)
     {
          for (int cou_Index=0;cou_Index<p_Length;cou_Index++)
          {
               if (cou_Index == 0){ in(p_Row, p_Cell, p_Submission[cou_Index]); continue; }
               ina(p_Row, p_Cell, p_Submission[cou_Index]);
          }
     }
     
     //Accepts an array to be input into a row.
     void in_A(int p_Row, int p_Cell, double * p_Submission, int p_Length)
     {
          for (int cou_Index=0;cou_Index<p_Length;cou_Index++)
          {
               if (cou_Index == 0){ in(p_Row, p_Cell, p_Submission[cou_Index]); continue; }
               ina(p_Row, p_Cell, p_Submission[cou_Index]);
          }
     }
     
     //Accepts an array to be input into a row.
     void in_A(int p_Row, int p_Cell, void ** p_Submission, int p_Length)
     {
          for (int cou_Index=0;cou_Index<p_Length;cou_Index++)
          {
               if (cou_Index == 0){ in(p_Row, p_Cell, p_Submission[cou_Index]); continue; }
               ina(p_Row, p_Cell, p_Submission[cou_Index]);
          }
     }
     
     
     
     
     //==--     INPUT TO THE GOAL TABLE
     
     
     //Accepts an input to the inputs motherfucker.
     void ing(int p_Row, int p_Cell, string p_Submission)
     {
          tbl_Input_Goal.set_String(p_Row, p_Cell, p_Submission);
     }
     
     //Accepts an input to the inputs motherfucker.
     void ing(int p_Row, int p_Cell, int p_Submission)
     {
          tbl_Input_Goal.set_Int(p_Row, p_Cell, p_Submission);
     }
     
     //Accepts an input to the inputs motherfucker.
     void ing(int p_Row, int p_Cell, double p_Submission)
     {
          tbl_Input_Goal.set_Float(p_Row, p_Cell, p_Submission);
     }
     
     //Accepts an input to the inputs motherfucker.
     void ing(int p_Row, int p_Cell, void * p_Submission)
     {
          tbl_Input_Goal.set_NR(p_Row, p_Cell, p_Submission);
     }
     
     
     //==--   Adds the submission to the given cell.
     //Accepts an input to the inputs motherfucker.
     void inga(int p_Row, int p_Cell, int p_Submission)
     {
          tbl_Input_Goal.add_Data_Int(p_Row, p_Cell, p_Submission);
     }
     
     //Accepts an input to the inputs motherfucker.
     void inga(int p_Row, int p_Cell, double p_Submission)
     {
          tbl_Input_Goal.add_Data_Float(p_Row, p_Cell, p_Submission);
     }
     
     //Accepts an input to the inputs motherfucker.
     void inga(int p_Row, int p_Cell, void * p_Submission)
     {
          tbl_Input_Goal.add_Data_NR(p_Row, p_Cell, p_Submission);
     }
     
     
     
     
     //Accepts an array to be input into a row.
     void inga_A(int p_Row, int p_Cell, int * p_Submission, int p_Length)
     {
          for (int cou_Index=0;cou_Index<p_Length;cou_Index++)
          {
               inga(p_Row, p_Cell, p_Submission[cou_Index]);
          }
     }
     
     //Accepts an array to be input into a row.
     void inga_A(int p_Row, int p_Cell, double * p_Submission, int p_Length)
     {
          for (int cou_Index=0;cou_Index<p_Length;cou_Index++)
          {
               inga(p_Row, p_Cell, p_Submission[cou_Index]);
          }
     }
     
     //Accepts an array to be input into a row.
     void inga_A(int p_Row, int p_Cell, void ** p_Submission, int p_Length)
     {
          for (int cou_Index=0;cou_Index<p_Length;cou_Index++)
          {
               inga(p_Row, p_Cell, p_Submission[cou_Index]);
          }
     }
     
     
     
     //Accepts an array to be input into a row.
     void ing_A(int p_Row, int p_Cell, int * p_Submission, int p_Length)
     {
          for (int cou_Index=0;cou_Index<p_Length;cou_Index++)
          {
               if (cou_Index == 0){ ing(p_Row, p_Cell, p_Submission[cou_Index]); continue; }
               inga(p_Row, p_Cell, p_Submission[cou_Index]);
          }
     }
     
     //Accepts an array to be input into a row.
     void ing_A(int p_Row, int p_Cell, double * p_Submission, int p_Length)
     {
          for (int cou_Index=0;cou_Index<p_Length;cou_Index++)
          {
               if (cou_Index == 0){ ing(p_Row, p_Cell, p_Submission[cou_Index]); continue; }
               inga(p_Row, p_Cell, p_Submission[cou_Index]);
          }
     }
     
     //Accepts an array to be input into a row.
     void ing_A(int p_Row, int p_Cell, void ** p_Submission, int p_Length)
     {
          for (int cou_Index=0;cou_Index<p_Length;cou_Index++)
          {
               if (cou_Index == 0){ ing(p_Row, p_Cell, p_Submission[cou_Index]); continue; }
               inga(p_Row, p_Cell, p_Submission[cou_Index]);
          }
     }
     //Returns data from a given constructs pattern output.
     
};

