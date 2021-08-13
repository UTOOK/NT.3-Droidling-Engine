//This class is the SSCC for working with multiple inputs.
class c_NT3_SSCC
{
private:
     
     
     //The Discharging_Buffers. One for each input.
     c_NT3_Data_Buffer * Discharging_Buffers;
     
public:
     
     //The constructs in the assembly.
     c_NT3_Construct_1D Constructs;
     
     //This construct is the SSCC that takes the treetops from the raw constructs as input.
     c_NT3_Construct_1D SSCC;
     
     //The name of the construct.
     string Name;
     
     
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
     
     //The treetops table to hold the treetops when building and charging.
     c_Raw_Table_1D tbl_Treetops;
     
     //The table to hold the charges when charging.
     c_Raw_Table_1D tbl_Charges;
     
     //Holds the positions when charging.
     c_Raw_Table_1D tbl_Positions;
     
     //Output tables for the patterns and charges, both left and right.
     c_Raw_Table_1D tbl_Output_Pattern_L;
     c_Raw_Table_1D tbl_Output_Pattern_R;
     c_Raw_Table_1D tbl_Output_Charges_L;
     c_Raw_Table_1D tbl_Output_Charges_R;
     c_Raw_Table_1D tbl_Output_Number_Of_Patterns_L;
     c_Raw_Table_1D tbl_Output_Number_Of_Patterns_R;
     c_Raw_Table_1D tbl_Output_Position_L;
     c_Raw_Table_1D tbl_Output_Position_R;
     c_Raw_Table_1D tbl_Output_RC_L;
     c_Raw_Table_1D tbl_Output_RC_R;
     c_Raw_Table_1D tbl_Output_Treetops_L;
     c_Raw_Table_1D tbl_Output_Treetops_R;
     
     //==--   SETTINGS
     
     //Whether or not to gather the outputs into charging buffers before the SSCC is discharged.
     int settings_Compress_SSCC_Outputs;
     
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
     
     double settings_Vision_Fog;
     
     int settings_Vision_Verify;
     c_Raw_Table_1D tbl_Verified_Output;
     
     //This controls how many outputs are read into the vision.
     int settings_Vision_Cap;
     
     c_NT3_SSCC()
     {
          Name = "Generic_Assembly";
          tbl_Input.Name = "Assembly_Input";
          tbl_Input_Goal.Name = "Assembly_Input_Goal";
          
          tbl_Vision_Output.Name = "Assembly_Vision";
          tbl_Vision_Output_Charge.Name = "Assembly_Vision_Charge";
          tbl_Verified_Output.Name = "Assembly_Vision_Verified_Output";

          tbl_Treetops.Name = "Assembly_SSCC_Treetops";
          tbl_Charges.Name = "Assembly_SSCC_Charges";
          tbl_Positions.Name = "Assembly_SSCC_Positions";
     
          tbl_Output_Pattern_L.Name = "Assembly_Output_Pattern_L";
          tbl_Output_Pattern_R.Name = "Assembly_Output_Pattern_R";
          tbl_Output_Charges_L.Name = "Assembly_Output_Charges_L";
          tbl_Output_Charges_R.Name = "Assembly_Output_Charges_R";
          tbl_Output_Number_Of_Patterns_L.Name = "Assembly_Output_Number_Of_Patterns_L";
          tbl_Output_Number_Of_Patterns_R.Name = "Assembly_Output_Number_Of_Patterns_R";
          tbl_Output_Position_L.Name = "Assembly_Output_Position_L";
          tbl_Output_Position_R.Name = "Assembly_Output_Position_R";
          tbl_Output_RC_L.Name = "Assembly_Output_RC_L";
          tbl_Output_RC_R.Name = "Assembly_Output_RC_R";
          tbl_Output_Treetops_L.Name = "Assembly_Output_Treetop_L";
          tbl_Output_Treetops_R.Name = "Assembly_Output_Treetop_R";
          
          SSCC.set_Name("SSCC");
          SSCC.settings_Action_Potential_Threshold = 0;
          SSCC.settings_Modifier_Charge = 1;
          Discharging_Buffers = NULL;

          
          string tmp_Construct_Name = Name + "___Raw";
          Constructs.set_Name(tmp_Construct_Name);
          Constructs.settings_Tier_Is_Floating = 1;
          Constructs.settings_Tier_Depth = .9;
          Constructs.settings_Cross_CAN_Charging = 1;
          
          settings_APT_Inc = .05;
          settings_APT_LInc = .01;
          
          settings_MC_Inc = .05;
          settings_MC_LInc = .01;
          
          settings_BC_Inc = 2500;
          settings_BC_LInc = 100;
          
          settings_FT_Inc = .1;
          settings_FT_LInc = .01;
          
          settings_Compress_SSCC_Outputs = 0;
          
          settings_Auto_Row_Limit = 0;
          
          settings_Accuracy_Upper = -1;
          
          settings_Vision_Fog = .5;
          
          Action_Potential_Threshold = 0.25f;
          Base_Charge = 10000;
     }
     
     ~c_NT3_SSCC()
     {
          cout << "\n ~c_NT3_SSCC " << Name << " " << this << ".........."; cout.flush();
          
          delete [] Discharging_Buffers;
          Discharging_Buffers = NULL;
          
          
          
          cout << "\n ~~~c_NT3_SSCC " << Name << " " << this << ".........."; cout.flush();
     }
     
     //Sets the name of the assembly.
     void set_Name(string p_Name)
     {
          Name = p_Name;
          tbl_Input.Name = Name + "__Input";
          tbl_Input_Goal.Name = Name + "__Input_Goal";
          
          string tmp_Construct_Name = "";
          tmp_Construct_Name = Name + "___Input_0";
          Constructs.set_Name(tmp_Construct_Name);
     }
     
     ////==-----------------------+
     //==--   EVAL FUNCTIONS
     ////==-----------------------+
     
     //Eval with no RC.
     void Eval()
     { 
          /*
          //Build_Query(); 
          SSCC.reset_Inputs();
          
          for (int cou_Index=0;cou_Index<tbl_Input.Number_Of_Rows;cou_Index++)
          {
               Constructs.Eval(); ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
               //Gather treetops
               gather_SSCC_Eval_Input_L(cou_Index);
               gather_SSCC_Eval_Input_R(cou_Index);
               Constructs.reset_Outputs();
          }
          gather_Highest_Position_L();
          gather_Highest_Position_R();
          
          
          //~cout << "\n\n\n\n\n\n SSCC Eval";
          
          
          
          SSCC.Eval_Aux_Inputs();
          //~SSCC.output_Input_Tables();
          //~SSCC.output_Output_Tables();
          
          
          if (settings_Compress_SSCC_Outputs == 0)
          {
               discharge_SSCC_Treetops_No_Compress_L();
               discharge_SSCC_Treetops_No_Compress_R();
          }
          
          if (settings_Compress_SSCC_Outputs == 1)
          {
               discharge_SSCC_Treetops_L();
               discharge_SSCC_Treetops_R();
          }
          
          //output_Output_Tables();
           * */
     } 
     
     //Eval with RC.
     void Eval_RC()
     { 
          /*
          //~cout << "\n gather_Inputs()";
          //~tbl_Input.output_C();
          c_Raw_Table_Cell_1D * tmp_Treetops = NULL;
          reset_SSCC_Tables();
          
          for (int cou_Index=0;cou_Index<tbl_Input.Number_Of_Rows;cou_Index++)
          {
               //Pass the input row to the construct.
               Constructs.tbl_Input.set_Row(0, tbl_Input.Rows[cou_Index]); 
               //~cout << "\n Constructs[" << cou_Index << "]->Build();";
               Constructs.Build();///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
               //Gather Treetops, treetops are gathered into a single cell for the building of the vanilla patterns.
               
               tmp_Treetops = Constructs.tbl_Treetops.get_Cell_Ref(0, 0);
               
               if (tmp_Treetops == NULL){ continue; }
               for (int cou_D=0;cou_D<tmp_Treetops->Length;cou_D++)
               {
                    tbl_Treetops.add_Data(0, 0, tmp_Treetops->Data[cou_D]);
               }
               
               Constructs.reset_Outputs();
          }
          gather_Highest_Position_L();
          gather_Highest_Position_R();
          
          
          //~cout << "\n gather_SSCC_Input()";
          gather_SSCC_Input();
          
          //~cout << "\n SSCC.Build();";
          SSCC.Build();
          
          
          SSCC.reset_Inputs();
          
          gather_SSCC_Eval_Input_L();
          gather_SSCC_Eval_Input_R();
          
          SSCC.Eval_Aux_Inputs_RC();
          //SSCC.output_Input_Tables();
          //SSCC.output_Output_Tables();
          
          if (settings_Compress_SSCC_Outputs == 0)
          {
               discharge_SSCC_Treetops_No_Compress_L();
               discharge_SSCC_Treetops_No_Compress_R();
          }
          
          if (settings_Compress_SSCC_Outputs == 1)
          {
               discharge_SSCC_Treetops_L();
               discharge_SSCC_Treetops_R();
          }
          //output_Output_Tables();
          */
     }
     
     //Eval with RC.
     void Eval_Freely()
     { 
          /*
          //Build_Query(); 
          for (int cou_Index=0;cou_Index<tbl_Input.Number_Of_Rows;cou_Index++)
          {
               if (Constructs.sauto_Use_RC)
               {
                    Constructs.Eval_RC();///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    Constructs.reset_Outputs();
                    //Gather treetops
               }
               else
               {
                    Constructs.Eval();///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    Constructs.reset_Outputs();
                    //Gather treetops
               }
          }
          gather_Highest_Position_L();
          gather_Highest_Position_R();
          
          
          SSCC.reset_Inputs();
          
          gather_SSCC_Eval_Input_L();
          gather_SSCC_Eval_Input_R();
          
          if (SSCC.sauto_Use_RC)
          {
               SSCC.Eval_Aux_Inputs_RC();
          }
          else
          {
               SSCC.Eval_Aux_Inputs();
          }
          //SSCC.output_Input_Tables();
          //SSCC.output_Output_Tables();
          
          if (settings_Compress_SSCC_Outputs == 0)
          {
               discharge_SSCC_Treetops_No_Compress_L();
               discharge_SSCC_Treetops_No_Compress_R();
          }
          
          if (settings_Compress_SSCC_Outputs == 1)
          {
               discharge_SSCC_Treetops_L();
               discharge_SSCC_Treetops_R();
          }
          //output_Output_Tables();
           * */
     }
     
     //Eval with RC.
     void Eval_Freely_Poly()
     { 
          //Build_Query(); 
          SSCC.reset_Inputs();
          reset_Output_Tables();
          
          for (int cou_Index=0;cou_Index<tbl_Input.Number_Of_Rows;cou_Index++)
          {
               if (Constructs.sauto_Use_RC)
               {
                    Constructs.tbl_Input.set_Row(0, tbl_Input.Rows[cou_Index]); 
                    Constructs.Eval_RC();
                    //Gather Treetops
                    
                    gather_SSCC_Eval_Input_L(cou_Index);
                    gather_SSCC_Eval_Input_R(cou_Index);
                    
                    discharge_C_Treetops_No_Compress_L(cou_Index);
                    Constructs.reset_Outputs();
               }
               else
               {
                    Constructs.tbl_Input.set_Row(0, tbl_Input.Rows[cou_Index]); 
                    Constructs.Eval();
                    //Gather Treetops
                    
                    gather_SSCC_Eval_Input_L(cou_Index);
                    gather_SSCC_Eval_Input_R(cou_Index);
                    
                    discharge_C_Treetops_No_Compress_L(cou_Index);
                    Constructs.reset_Outputs();
               }
          }
          
          tbl_Verified_Output.reset();
          
          gather_Highest_Position_L();
          gather_Highest_Position_R();
          
          if (SSCC.sauto_Use_RC)
          {
               //SSCC.Eval_Aux_Poly_Inputs_Inclusively_RC(1); ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
               SSCC.Eval_Aux_Poly_Inputs_RC(1);
               //SSCC.Eval_Aux_Inputs_RC();
          }
          else
          {
               //SSCC.Eval_Aux_Poly_Inputs_Inclusively(1);
               SSCC.Eval_Aux_Poly_Inputs(1);
               //SSCC.Eval_Aux_Inputs();
          }
          
          //SSCC.output_PCAN_State();
          
          //SSCC.output_Input_Tables();
          //SSCC.output_Output_Tables();
          
          //SSCC.tbl_Pattern_Output_L.output_C();
          //SSCC.tbl_Charge_Output_L.output_I();
          
          discharge_SSCC_Treetops_No_Compress_L();
          discharge_SSCC_Treetops_No_Compress_R();
          
          //output_Output_Tables();          
          gather_Vision_Verified_Table();
     }
     
     
     ////==-----------------------+
     //==--   BUILDING FUNCTIONS
     ////==-----------------------+
     
     //Builds full CAN.
     void Build()
     { 
          //~cout << "\n gather_Inputs()";
          //~tbl_Input.output_C();
          c_Raw_Table_Cell_1D * tmp_Treetops = NULL;
          reset_SSCC_Tables();
          
          for (int cou_Index=0;cou_Index<tbl_Input.Number_Of_Rows;cou_Index++)
          {
               //Pass the input row to the construct.
               Constructs.tbl_Input.set_Row(0, tbl_Input.Rows[cou_Index]); 
               //~cout << "\n Constructs[" << cou_Index << "]->Build();";
               Constructs.Build();
               //Gather Treetops, treetops are gathered into a single cell for the building of the vanilla patterns.
               
               tmp_Treetops = Constructs.tbl_Treetops.get_Cell_Ref(0, 0);
               
               if (tmp_Treetops == NULL){ continue; }
               for (int cou_D=0;cou_D<tmp_Treetops->Length;cou_D++)
               {
                    tbl_Treetops.add_Data(0, 0, tmp_Treetops->Data[cou_D]);
               }
               
               Constructs.reset_Outputs();
          }
          
          //~cout << "\n gather_SSCC_Input()";
          gather_SSCC_Input();
          
          //~cout << "\n SSCC.Build();";
          SSCC.Build();
     } 
     
     //Builds RC CAN.
     void Build_RC()
     { 
          //~cout << "\n gather_Inputs()";
          //~tbl_Input.output_C();
          c_Raw_Table_Cell_1D * tmp_Treetops = NULL;
          reset_SSCC_Tables();
          
          for (int cou_Index=0;cou_Index<tbl_Input.Number_Of_Rows;cou_Index++)
          {
               //Pass the input row to the construct.
               Constructs.tbl_Input.set_Row(0, tbl_Input.Rows[cou_Index]); 
               //~cout << "\n Constructs[" << cou_Index << "]->Build();";
               Constructs.Build();///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
               //Gather Treetops, treetops are gathered into a single cell for the building of the vanilla patterns.
               
               tmp_Treetops = Constructs.tbl_Treetops.get_Cell_Ref(0, 0);
               
               if (tmp_Treetops == NULL){ continue; }
               for (int cou_D=0;cou_D<tmp_Treetops->Length;cou_D++)
               {
                    tbl_Treetops.add_Data(0, 0, tmp_Treetops->Data[cou_D]);
               }
               
               Constructs.reset_Outputs();
          }
          
          //~cout << "\n gather_SSCC_Input()";
          gather_SSCC_Input();
          
          //~cout << "\n SSCC.Build();";
          SSCC.Build_RC();
          //SSCC.output_CAN_RC();
          //system("PAUSE > NULL");
     } 
     
     //Builds for query.
     void Build_Query(int p_Row)
     {
          //Pass the input row to the construct.
          Constructs.tbl_Input.set_Row(0, tbl_Input.Rows[p_Row]); 
          Constructs.Build_Query();
     } 
     
     
     
     //Gatheres the inputs into the raw constructs, creates constructs if needed.
     
     ////==--------------------------+
     //==--   DISCHARGING FUNCTIONS
     ////==--------------------------+
     
     //Discharges the SSCC treetops back through the raw constructs by first setting up the charging buffers. 
     //Then is submits the nodes from the SSCC outputs to the charging buffers.
     //After the nodes have all been read in the buffers in the constructs are gathered() then discharged().
     //It does this by looping through the SSCC output tables and extracting the NID|CID, charge, and position.
     //These are then submitted to the appropriate constructs.
     void discharge_SSCC_Treetops_L()
     {
          /*
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
          if (SSCC.tbl_Pattern_Output_L.Number_Of_Rows == 0){ return; }
          
          //Each row in the SSCC output is read into the row in the Construct output. Row for Row.
          for (int cou_R=0;cou_R<SSCC.tbl_Pattern_Output_L.Number_Of_Rows;cou_R++)
          {
               //Gather the number of buffers to hold the discharged nodes.
               tmp_Number_Of_Buffers = 0;
               
               for (int cou_C=0;cou_C<SSCC.tbl_Pattern_Output_L.Rows[cou_R]->Depth;cou_C++)
               {
                    tmp_Position = SSCC.tbl_Position_Output_L.get_I(cou_R, cou_C, 0);
                    if (tmp_Position > tmp_Number_Of_Buffers){ tmp_Number_Of_Buffers = tmp_Position; }
               }
               tmp_Number_Of_Buffers++;
               
               //Expand the buffers to accommodate the outputs.
               init_Discharging_Buffers(tmp_Number_Of_Buffers);
               
               for (int cou_C=0;cou_C<SSCC.tbl_Pattern_Output_L.Rows[cou_R]->Depth;cou_C++)
               {
                    
                    tmp_Charge = SSCC.tbl_Charge_Output_L.get(cou_R, cou_C, 0);
                    tmp_Position = SSCC.tbl_Position_Output_L.get_I(cou_R, cou_C, 0);
                         
                    //Each data bit in the cell is one pattern to return.
                    for (int cou_D=0;cou_D<SSCC.tbl_Pattern_Output_L.Rows[cou_R]->Cells[cou_C]->Length;cou_D++)
                    {
                         tmp_NIDCID = SSCC.tbl_Pattern_Output_L.get(cou_R, cou_C, cou_D);
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
                         
                         Constructs.discharge_Treetop_By_NID_L(cou_P, tmp_NID, tmp_Charge, cou_R);
                         
                         tmp_LL = tmp_LL->Next;
                    }
               }
          }
          */
     }
     
     void discharge_SSCC_Treetops_R()
     {
          /*
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
          if (SSCC.tbl_Pattern_Output_R.Number_Of_Rows == 0){ return; }
          
          //Each row in the SSCC output is read into the row in the Construct output. Row for Row.
          for (int cou_R=0;cou_R<SSCC.tbl_Pattern_Output_R.Number_Of_Rows;cou_R++)
          {
               //Gather the number of buffers to hold the discharged nodes.
               tmp_Number_Of_Buffers = 0;
               for (int cou_C=0;cou_C<SSCC.tbl_Pattern_Output_R.Rows[cou_R]->Depth;cou_C++)
               {
                    tmp_Position = SSCC.tbl_Position_Output_R.get_I(cou_R, cou_C, 0);
                    if (tmp_Position > tmp_Number_Of_Buffers){ tmp_Number_Of_Buffers = tmp_Position; }
               }
               tmp_Number_Of_Buffers++;
               
               //Expand the buffers to accommodate the outputs.
               init_Discharging_Buffers(tmp_Number_Of_Buffers);
               
               for (int cou_C=0;cou_C<SSCC.tbl_Pattern_Output_R.Rows[cou_R]->Depth;cou_C++)
               {
                    
                    tmp_Charge = SSCC.tbl_Charge_Output_R.get(cou_R, cou_C, 0);
                    tmp_Position = SSCC.tbl_Position_Output_R.get_I(cou_R, cou_C, 0);
                         
                    //Each data bit in the cell is one pattern to return.
                    for (int cou_D=0;cou_D<SSCC.tbl_Pattern_Output_R.Rows[cou_R]->Cells[cou_C]->Length;cou_D++)
                    {
                         tmp_NIDCID = SSCC.tbl_Pattern_Output_R.get(cou_R, cou_C, cou_D);
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
                         
                         Constructs.discharge_Treetop_By_NID_R(cou_P, tmp_NID, tmp_Charge, cou_R);
                         
                         tmp_LL = tmp_LL->Next;
                    }
               }
          }
          */
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
     
     //Bubbles sorts the output tables for limited output.
     void bubble_Sort_SSCC_Outputs_L()
     {
          //u_Data_3 tmp_Charge;
          //tmp_Charge.U = 0;
          
          //If the table is empty then return.
          if (SSCC.tbl_Pattern_Output_L.Number_Of_Rows == 0){ return; }
          
          //Each row in the SSCC output is read into the row in the Construct output. Row for Row.
          for (int cou_R=0;cou_R<SSCC.tbl_Pattern_Output_L.Number_Of_Rows;cou_R++)
          {
               //Discharge the outputs.
               //Each cell is discharged into the output row correlating with its position in the output string.
               for (int cou_C=0;cou_C<SSCC.tbl_Pattern_Output_L.Rows[cou_R]->Depth;cou_C++)
               {
               }
          }
     }
     
     void discharge_SSCC_Treetops_No_Compress_L(int p_Output_Cap = 0)
     {
          u_Data_3 tmp_NID;
          tmp_NID.U = 0;
          u_Data_3 tmp_Charge;
          tmp_Charge.U = 0;
          int tmp_Position = 0;
          
          int tmp_Cuttoff = Base_Charge * Action_Potential_Threshold;
          
          //If the table is empty then return.
          if (SSCC.tbl_Pattern_Output_L.Number_Of_Rows == 0){ return; }
          
          //Each row in the SSCC output is read into the row in the Construct output. Row for Row.
          for (int cou_R=0;cou_R<SSCC.tbl_Pattern_Output_L.Number_Of_Rows;cou_R++)
          {
               //Discharge the outputs.
               //Each cell is discharged into the output row correlating with its position in the output string.
               for (int cou_C=0;cou_C<SSCC.tbl_Pattern_Output_L.Rows[cou_R]->Depth;cou_C++)
               {
                    
                    tmp_Charge = SSCC.tbl_Charge_Output_L.get(cou_R, cou_C, 0);
                    tmp_Position = SSCC.tbl_Position_Output_L.get_I(cou_R, cou_C, 0);
                    
                    //cout << "\n if (" << tmp_Charge.I << " < " << tmp_Cuttoff << ")";
                    if (tmp_Charge.I < tmp_Cuttoff){ continue; }
                    
                    
                    
                    //Each data bit in the cell is one pattern to return.
                    for (int cou_D=0;cou_D<SSCC.tbl_Pattern_Output_L.Rows[cou_R]->Cells[cou_C]->Length;cou_D++)
                    {
                         tmp_NID = SSCC.tbl_Pattern_Output_L.get(cou_R, cou_C, cou_D);
                         
                         Constructs.discharge_Treetop_By_NID_Into_Given_Tables_L(tmp_Position, tmp_NID, tmp_Charge, cou_D, &tbl_Output_Pattern_L, &tbl_Output_Position_L, &tbl_Output_Charges_L, &tbl_Output_RC_L, &tbl_Output_Treetops_L);
                         //void discharge_Treetop_By_NID_Into_Given_Tables_L(int p_Position, u_Data_3 p_NID, u_Data_3 p_Charge, int p_Current_Input, &p_PTbl, &p_Pos_Tbl, &p_CTbl, &p_RCTbl, &p_TtTbl
                    }
               }
               tbl_Output_Number_Of_Patterns_L.set_Int(0, 0, SSCC.tbl_Pattern_Output_L.Rows[cou_R]->Depth);
          }
          /*
          tbl_Output_Pattern_L.reset();
          tbl_Output_Charges_L.reset();
          tbl_Output_Number_Of_Patterns_L.reset();
          */
     }
     
     void discharge_SSCC_Treetops_No_Compress_R()
     {
          /*
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
          if (SSCC.tbl_Pattern_Output_R.Number_Of_Rows == 0){ return; }
          
          //Each row in the SSCC output is read into the row in the Construct output. Row for Row.
          for (int cou_R=0;cou_R<SSCC.tbl_Pattern_Output_R.Number_Of_Rows;cou_R++)
          {
               for (int cou_C=0;cou_C<SSCC.tbl_Pattern_Output_R.Rows[cou_R]->Depth;cou_C++)
               {
                    
                    tmp_Charge = SSCC.tbl_Charge_Output_R.get(cou_R, cou_C, 0);
                    tmp_Position = SSCC.tbl_Position_Output_R.get_I(cou_R, cou_C, 0);
                         
                    if (tmp_Charge.I < tmp_Cuttoff){ continue; }
                    
                    //Each data bit in the cell is one pattern to return.
                    for (int cou_D=0;cou_D<SSCC.tbl_Pattern_Output_R.Rows[cou_R]->Cells[cou_C]->Length;cou_D++)
                    {
                         tmp_NIDCID = SSCC.tbl_Pattern_Output_R.get(cou_R, cou_C, cou_D);
                         tmp_NID.U = tmp_NIDCID.U * .01;
                         tmp_CID.U = tmp_NIDCID.U - (tmp_NID.U * 100);
                         
                         Constructs.discharge_Treetop_By_NID_R(tmp_Position, tmp_NID, tmp_Charge, cou_R);
                    }
               }
          }
          */
     }
     
          
     void discharge_C_Treetops_No_Compress_L(int p_Position)
     {          
          u_Data_3 tmp_NID;
          tmp_NID.U = 0;
          u_Data_3 tmp_Charge;
          tmp_Charge.U = 0;
          int tmp_Position = 0;
          
          //~cout << "\n\t gather_SSCC_Eval_Input_L()";
          if (Constructs.tbl_Treetops_Output_L.Number_Of_Rows == 0){ return; }
          
          //~cout << " -Gather_Treetops";
          for (int cou_Row=0;cou_Row<Constructs.tbl_Treetops_Output_L.Number_Of_Rows;cou_Row++)
          {
               for (int cou_Cell=0;cou_Cell<Constructs.tbl_Treetops_Output_L.Rows[cou_Row]->Depth;cou_Cell++)
               {
                    //if ((Constructs.tbl_Treetops_Output_L.Rows[cou_Row]->Cells[cou_Cell]->Data[0]).U == 0){ continue; }
                    tmp_NID.U = (Constructs.tbl_Treetops_Output_L.Rows[cou_Row]->Cells[cou_Cell]->Data[0]).U;
                    tmp_Charge.U = (Constructs.tbl_Charge_Output_L.Rows[cou_Row]->Cells[cou_Cell]->Data[0]).U;
                    tmp_Position = (Constructs.tbl_Position_Output_L.Rows[cou_Row]->Cells[cou_Cell]->Data[0]).U;
                    
                    Constructs.discharge_Treetop_By_NID_Into_Given_Tables_L(tmp_Position, tmp_NID, tmp_Charge, p_Position, &tbl_Output_Pattern_L, &tbl_Output_Position_L, &tbl_Output_Charges_L, &tbl_Output_RC_L, &tbl_Output_Treetops_L);
               }
          }
          /*
          tbl_Output_Pattern_L.reset();
          tbl_Output_Charges_L.reset();
          tbl_Output_Number_Of_Patterns_L.reset();
          */
     }
     
     ////==-----------------------------------+
     //==--   INPUTS AND OUTPUT MANIPULATION
     ////==-----------------------------------+
     
     //Gathers the treetops from the built constructs and inputs them into the SSCC.
     void gather_SSCC_Input()
     {
          SSCC.reset_Inputs();
          tbl_Treetops.Rows[0]->output_I();
          SSCC.tbl_Input.set_Row(0, tbl_Treetops.Rows[0]);
     }
     
     //Gathers the treetops and charges from the evaluated constructs into the SSCC. Left.
     void gather_SSCC_Eval_Input_L(int p_Index)
     {
          //~cout << "\n\t gather_SSCC_Eval_Input_L()";
          if (Constructs.tbl_Treetops_Output_L.Number_Of_Rows == 0){ return; }
          
          //~cout << " -Gather_Treetops";
          for (int cou_Row=0;cou_Row<Constructs.tbl_Treetops_Output_L.Number_Of_Rows;cou_Row++)
          {
               for (int cou_Cell=0;cou_Cell<Constructs.tbl_Treetops_Output_L.Rows[cou_Row]->Depth;cou_Cell++)
               {
                    //if ((Constructs.tbl_Treetops_Output_L.Rows[cou_Row]->Cells[cou_Cell]->Data[0]).U == 0){ continue; }
                    //This must be given a CID for the positioning in the Poly input.
                    SSCC.tbl_Input_Aux_L.add_Data_Int(0, cou_Row, (((Constructs.tbl_Treetops_Output_L.Rows[cou_Row]->Cells[cou_Cell]->Data[0]).U * 100) + p_Index));
                    //SSCC.tbl_Input_Aux_L.add_Data_Int(0, cou_Row, (Constructs.tbl_Treetops_Output_L.Rows[cou_Row]->Cells[cou_Cell]->Data[0]).U);
                    SSCC.tbl_Input_Charges_L.add_Data_Int(0, cou_Row, (((Constructs.tbl_Charge_Output_L.Rows[cou_Row]->Cells[cou_Cell]->Data[0]).U)));
                    SSCC.tbl_Input_Position_L.add_Data_Int(0, cou_Row, (((Constructs.tbl_Position_Output_L.Rows[cou_Row]->Cells[cou_Cell]->Data[0]).U)));
               }
          }
     }
     
     //Gathers the highest index to use in the SSCC input position table. Now separate fom the gather() because gather() now does one index at a time
     //whereas the gather_Position() is across all of the indexes.
     void gather_Highest_Position_L()
     {
          int tmp_Input_Depth = 0;
          int tmp_Input_Depth_New = 0;
          
          //~cout << " -Set_SSCC_Depths";
          if (SSCC.tbl_Input_Position_L.Number_Of_Rows == 0){ return; }
          
          for (int cou_Cell=0;cou_Cell<SSCC.tbl_Input_Position_L.Rows[0]->Depth;cou_Cell++)
          {
               tmp_Input_Depth = 0;
               tmp_Input_Depth_New = 0;
               for (int cou_D=0;cou_D<SSCC.tbl_Input_Position_L.Rows[0]->Cells[cou_Cell]->Length;cou_D++)
               {
                    tmp_Input_Depth_New = (SSCC.tbl_Input_Position_L.Rows[0]->Cells[cou_Cell]->Data[cou_D]).I + 1;
                    if (tmp_Input_Depth < tmp_Input_Depth_New){ tmp_Input_Depth = tmp_Input_Depth_New; }
               }
               SSCC.tbl_Input_Depths_L.add_Data_Int(0, cou_Cell, tmp_Input_Depth);
          }
     }
     
     //Gathers the treetops and charges from the evaluated constructs into the SSCC. Right.
     void gather_SSCC_Eval_Input_R(int p_Index)
     {
          //~cout << "\n\t gather_SSCC_Eval_Input_R()";
          
          for (int cou_Row=0;cou_Row<Constructs.tbl_Treetops_Output_R.Number_Of_Rows;cou_Row++)
          {
               for (int cou_Cell=0;cou_Cell<Constructs.tbl_Treetops_Output_R.Rows[cou_Row]->Depth;cou_Cell++)
               {
                    //if ((Constructs.tbl_Treetops_Output_R.Rows[cou_Row]->Cells[cou_Cell]->Data[0]).U == 0){ continue; }
                    SSCC.tbl_Input_Aux_R.add_Data_Int(0, cou_Row, (((Constructs.tbl_Treetops_Output_R.Rows[cou_Row]->Cells[cou_Cell]->Data[0]).U * 100) + p_Index));
                    SSCC.tbl_Input_Charges_R.add_Data_Int(0, cou_Row, (((Constructs.tbl_Charge_Output_R.Rows[cou_Row]->Cells[cou_Cell]->Data[0]).U)));
                    SSCC.tbl_Input_Position_R.add_Data_Int(0, cou_Row, (((Constructs.tbl_Position_Output_R.Rows[cou_Row]->Cells[cou_Cell]->Data[0]).U)));
               }
          }
          
     }
     
     //Gathers the highest index to use in the SSCC input position table. Now separate fom the gather() because gather() now does one index at a time
     //whereas the gather_Position() is across all of the indexes.
     void gather_Highest_Position_R()
     {
          if (SSCC.tbl_Input_Position_R.Number_Of_Rows == 0){ return; }
          
          int tmp_Input_Depth = 0;
          int tmp_Input_Depth_New = 0;
          
          for (int cou_Cell=0;cou_Cell<SSCC.tbl_Input_Position_R.Rows[0]->Depth;cou_Cell++)
          {
               tmp_Input_Depth = 0;
               tmp_Input_Depth_New = 0;
               for (int cou_D=0;cou_D<SSCC.tbl_Input_Position_R.Rows[0]->Cells[cou_Cell]->Length;cou_D++)
               {
                    tmp_Input_Depth_New = (SSCC.tbl_Input_Position_R.Rows[0]->Cells[cou_Cell]->Data[cou_D]).I + 1;
                    if (tmp_Input_Depth < tmp_Input_Depth_New){ tmp_Input_Depth = tmp_Input_Depth_New; }
               }
               SSCC.tbl_Input_Depths_R.add_Data_Int(0, cou_Cell, tmp_Input_Depth);
          }
     }

     //Resets the input and output tables.
     void reset()
     {
          tbl_Input.reset();
          tbl_Input_Goal.reset();
          
          Constructs.reset_Outputs();
          
          SSCC.reset_Inputs();
          SSCC.reset_Outputs();
     }
     
     //Resets the tables for the SSCC
     void reset_SSCC_Tables()
     {
          tbl_Charges.reset();
          tbl_Treetops.reset();
          tbl_Positions.reset();
     }
     
     //Resets the output tables for the SSCC
     void reset_Output_Tables()
     {
          tbl_Output_Pattern_L.reset();
          tbl_Output_Pattern_R.reset();
          tbl_Output_Charges_L.reset();
          tbl_Output_Charges_R.reset();
          tbl_Output_Number_Of_Patterns_L.reset();
          tbl_Output_Number_Of_Patterns_R.reset();
          tbl_Output_Position_L.reset();
          tbl_Output_Position_R.reset();
          tbl_Output_RC_L.reset();
          tbl_Output_RC_R.reset();
          tbl_Output_Treetops_L.reset();
          tbl_Output_Treetops_R.reset();
          tbl_Vision_Output.reset();
          tbl_Vision_Output_Charge.reset();
     }
     
     ////==---------------------------+
     //==--   RAW CONSTRUCT SETTINGS
     ////==---------------------------+
     
     void set_Left_Leg(bool p_Setting, int p_SSCC = 0)
     {  
          if (p_SSCC == 1 || p_SSCC == 0){ Constructs.setting_Charge_L = p_Setting; }
          
          if (p_SSCC){ SSCC.setting_Charge_L = p_Setting; }
     }
     
     void set_Right_Leg(bool p_Setting, int p_SSCC = 0)
     {  
          if (p_SSCC == 1 || p_SSCC == 0){ Constructs.setting_Charge_R = p_Setting; }
          
          if (p_SSCC){ SSCC.setting_Charge_R = p_Setting; }
     }
     
     void set_Tier_Is_Floating(int p_Setting, int p_SSCC = 0)
     {  
          if (p_SSCC == 1 || p_SSCC == 0){ Constructs.settings_Tier_Is_Floating = p_Setting; }
          
          if (p_SSCC){ SSCC.settings_Tier_Is_Floating = p_Setting; }
     }
          
     void set_Floating_Tier_Depth(int p_Setting, int p_SSCC = 0)
     {  
          if (p_SSCC == 1 || p_SSCC == 0){ Constructs.settings_Tier_Depth = p_Setting; }
          
          if (p_SSCC){ SSCC.settings_Tier_Depth = p_Setting; }
     }
     
     void set_Tier(int p_Setting, int p_SSCC = 0)
     {  
          if (p_SSCC == 1 || p_SSCC == 0){ Constructs.settings_Tier = p_Setting; }
          
          if (p_SSCC){ SSCC.settings_Tier = p_Setting; }
     }
     
     void set_Action_Potential_Threshold(double p_Setting, int p_SSCC = 0)
     {  
          if (p_SSCC == 1 || p_SSCC == 0){ Constructs.settings_Action_Potential_Threshold = p_Setting; }
          
          if (p_SSCC){ SSCC.settings_Action_Potential_Threshold = p_Setting; }
     }
     void set_Base_Charge(int p_Setting, int p_SSCC = 0)
     {  
          if (p_SSCC == 1 || p_SSCC == 0){ Constructs.settings_Base_Charge = p_Setting; }
               
          if (p_SSCC){ SSCC.settings_Base_Charge = p_Setting; }
     }
     void set_Modifier_Charge(double p_Setting, int p_SSCC = 0)
     {  
          if (p_SSCC == 1 || p_SSCC == 0){ Constructs.settings_Modifier_Charge = p_Setting; }
          
          if (p_SSCC){ SSCC.settings_Modifier_Charge = p_Setting; }
     }
     void set_Input_Charging_Depth(double p_Setting, int p_SSCC = 0)
     {  
          if (p_SSCC == 1 || p_SSCC == 0){ Constructs.settings_Input_Charging_Depth = p_Setting; }
          
          if (p_SSCC){ SSCC.settings_Input_Charging_Depth = p_Setting; }
     }
     void set_RC(int p_Setting, int p_SSCC = 0)
     {  
          if (p_SSCC == 1 || p_SSCC == 0){ Constructs.sauto_Use_RC = p_Setting; }
          
          if (p_SSCC){ SSCC.sauto_Use_RC = p_Setting; }
     }
     
     ////==-----------+
     //==--   VISION
     ////==-----------+
          
     void gather_Vision_Verified_Table()
     {
          //If the table is empty then return.
          if (tbl_Output_Pattern_L.Number_Of_Rows == 0){ return; }
          
          for (int cou_R=0;cou_R<tbl_Output_Pattern_L.Number_Of_Rows;cou_R++)
          {
               for (int cou_C=0;cou_C<tbl_Output_Pattern_L.Rows[cou_R]->Depth;cou_C++)
               {
                    tbl_Verified_Output.set_Int(cou_R, cou_C, 1);
               }
          }
     }
     
     //Determines the deviance of the current vision cell.
     double gather_Vision_Deviance(c_Raw_Table_Cell_1D * p_Cell_Vision, c_Raw_Table_Cell_1D * p_Cell_Input)
     {
          
          if (p_Cell_Vision == NULL || p_Cell_Input == NULL){ return 5000; }
          double tmp_Len = p_Cell_Input->Length;
          double tmp_Deviance = 0;
          
          cout << "\n\n\n I->"; p_Cell_Input->output_F(); cout << "                                   ";
          cout << "\n V->"; p_Cell_Vision->output_F();  cout << "                                   ";
          
          if (tmp_Len >= p_Cell_Vision->Length){ return 5000; }
          
          //Loop through the data in the cells doing a comparision.
          for (int cou_D=0;cou_D<tmp_Len;cou_D++)
          {
               tmp_Deviance += absoluted(get_Difference(p_Cell_Vision->Data[cou_D].F, p_Cell_Input->Data[cou_D].F));
          }
          tmp_Deviance /= tmp_Len;
          
          
          c_Shipyard_Line_Graph tmp_Graph;
          for (int cou_D=0;cou_D<tmp_Len;cou_D++)
          {
               tmp_Graph.submit_Data_Point(0, cou_D, p_Cell_Input->Data[cou_D].F);
               tmp_Graph.submit_Data_Point(1, cou_D, p_Cell_Vision->Data[cou_D].F);
          }
          
          cout << "\n";
          tmp_Graph.output_Minimal(get_Console_Cursor_X(), get_Console_Cursor_Y(), 10, 5);
          
          cout << "\n ->"; odbl(0, 13, tmp_Deviance);
          //system("PAUSE > NULL");
          
          
          return tmp_Deviance;
     }
     
     //Determines the deviance of the current vision cell.
     double gather_Vision_Deviance_I(c_Raw_Table_Cell_1D * p_Cell_Vision, c_Raw_Table_Cell_1D * p_Cell_Input)
     {
          
          if (p_Cell_Vision == NULL || p_Cell_Input == NULL){ return 5000; }
          double tmp_Len = p_Cell_Input->Length;
          double tmp_Deviance = 0;
          
          //cout << "\n\n\n I->"; p_Cell_Input->output_I(); cout << "                                   ";
          //cout << "\n V->"; p_Cell_Vision->output_I();  cout << "                                   ";
          
          if (tmp_Len >= p_Cell_Vision->Length){ cout << " RRRR"; return 5000; }
          
          //Loop through the data in the cells doing a comparision.
          for (int cou_D=0;cou_D<tmp_Len;cou_D++)
          {
               tmp_Deviance += absoluted(get_Difference(p_Cell_Vision->Data[cou_D].I, p_Cell_Input->Data[cou_D].I));
          }
          tmp_Deviance /= tmp_Len;
          
          /*
          c_Shipyard_Line_Graph tmp_Graph;
          for (int cou_D=0;cou_D<p_Cell_Input->Length;cou_D++)
          {
               tmp_Graph.submit_Data_Point(0, cou_D, p_Cell_Input->Data[cou_D].I);
          }
          for (int cou_D=0;cou_D<p_Cell_Vision->Length;cou_D++)
          {
               tmp_Graph.submit_Data_Point(1, cou_D, p_Cell_Vision->Data[cou_D].I);
          }
          
          cout << "\n";
          tmp_Graph.output_Minimal(get_Console_Cursor_X(), get_Console_Cursor_Y(), 10, 5);
          
          cout << "\n ->"; odbl(0, 13, tmp_Deviance);
          //system("PAUSE > NULL");
          */
          
          return tmp_Deviance;
     }
     
     
     //Gathers a weighted average vision.
     void gather_Vision_Cell_I(c_Raw_Table_Cell_1D * p_Cell_Vision, c_Raw_Table_Cell_1D * p_Cell_Output, double p_Charge, int p_Position = 0)
     {
          if (p_Cell_Vision == NULL || p_Cell_Output == NULL){ return; }
          int tmp_Len = p_Cell_Output->Length;
          
          //Loop through the data in the cells doing a comparision.
          for (int cou_D=0;cou_D<tmp_Len;cou_D++)
          {
               p_Cell_Vision->Data[cou_D].F += (p_Cell_Output->Data[cou_D].I * p_Charge);
               //p_Cell_Vision->Data[cou_D].F += (p_Cell_Output->Data[cou_D].I);
          }
          
          return;
     }
     
     c_Raw_Table_Cell_1D * gather_Vision_Row_A(int p_Row)
     {
          return NULL;
     }
     
     //Gathers the stats for a row.
     c_Raw_Table_Cell_1D * gather_Vision_Row_I(int p_Row)
     {
          //The highest charge in the row, used for getting the weighted averages to add.
          long long int tmp_Total_Charge = 0;
          
          //Get the goal cell and the output row.
          c_Raw_Table_Row_1D * output_Row = tbl_Output_Pattern_L.get_Row_Ref(p_Row);
          c_Raw_Table_Row_1D * output_Charge_Row = tbl_Output_Charges_L.get_Row_Ref(p_Row);
          c_Raw_Table_Row_1D * output_Position_Row = tbl_Output_Position_L.get_Row_Ref(p_Row);
          c_Raw_Table_Row_1D * output_Verified_Row = tbl_Verified_Output.get_Row_Ref_Hard(p_Row);
          c_Raw_Table_Cell_1D * Vision_Cell = tbl_Vision_Output.get_Cell_Ref_Hard(p_Row, 0);
          
          Vision_Cell->reset();
          
          //If there are no charges return;
          if (output_Row == NULL){ return NULL; }
          if (output_Charge_Row == NULL){ return NULL; }
          
          //Gather the total charge.
          for (int cou_C=0;cou_C<output_Charge_Row->Depth;cou_C++)
          {
               if (output_Verified_Row->Cells[cou_C]->Data[0].I == 0){ continue; }
               if (gather_Vision_Deviance_I(output_Row->Cells[cou_C], tbl_Input.get_Cell_Ref_Hard(p_Row, 0)) >= settings_Vision_Fog){ output_Verified_Row->Cells[cou_C]->Data[0].I = 0; continue; }
               tmp_Total_Charge += output_Charge_Row->Cells[cou_C]->Data[0].I;
          }
          
          if (tmp_Total_Charge == 0){ return NULL; }
          
          //Sets the vision charge.
          tbl_Vision_Output_Charge.set_Float(p_Row, 0, tmp_Total_Charge);
          
          //If either are NULL return;
          if (output_Row == NULL){ return NULL; }
          
          if (output_Row->Depth >= 1)
          {
               Vision_Cell->resize(output_Row->Cells[0]->Length);
          }
          for (int cou_C=0;cou_C<output_Row->Depth;cou_C++)
          {
               gather_Vision_Cell_I(Vision_Cell, output_Row->Cells[cou_C], (double (output_Charge_Row->Cells[cou_C]->Data[0].I) / tmp_Total_Charge), output_Position_Row->Cells[cou_C]->Data[0].I);
          }
          
          
          return Vision_Cell;
          
          return NULL;
     }
     
     c_Raw_Table_Cell_1D * gather_Vision_Row(int p_Row)
     {
          if (Constructs.sauto_Type == 0){ return gather_Vision_Row_I(p_Row); }
          if (Constructs.sauto_Type == 1){ return gather_Vision_Row_A(p_Row); }
          return NULL;
     }
     
     c_Raw_Table_Cell_1D * gather_Vision()
     {
          for (int cou_Index=0;cou_Index<tbl_Input_Goal.Number_Of_Rows;cou_Index++)
          {
               if (Constructs.sauto_Type == 0){ gather_Vision_Row_I(cou_Index); }
               if (Constructs.sauto_Type == 1){ gather_Vision_Row_A(cou_Index); }
          }
          return NULL;
     }
     
     
     
     ////==---------------------------------+
     //==--   ACCURACY GATHERING FUNCTIONS
     ////==---------------------------------+
     
          //Gather accuracy for every row.
     double gather_VAccuracy(int p_Limit = 0)
     {
          //gather_Vision();
          
          return 10000.0f;
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
          
          cout << "\n\n --==   SSCC Input Tables  ==--";
          SSCC.output_Input_Tables();
          
          cout << "\n\n --==   Construct Input Tables  ==--";
          if (p_Display == 0){ Constructs.output_Input_Table(); }
          if (p_Display == 1){ Constructs.output_Input_Table_I(); }
          if (p_Display == 2){ Constructs.output_Input_Table_F(); }
          if (p_Display == 3){ Constructs.output_Input_Table_NR(); }
     } 
     
     //Outputs the output tables. 
     void output_Output_Tables()
     {  
          cout << "\n\n --==   Assembly Output Tables  ==--";
          
          cout << "\n\n --==   Construct Output Tables  ==--";
          SSCC.output_Output_Tables();
          
          cout << "\n\n --==   Construct Output Tables  ==--";
          Constructs.output_Output_Tables();
          Constructs.output_Output_Tables_I();
          Constructs.output_Output_Tables_F();
     } 
     
     //Outputs the output tables. 
     void output_Output_Tables_I()
     {  
          cout << "\n\n --==   Assembly Output Tables  ==--";
          
          cout << "\n\n --==   Construct Output Tables  ==--";
          SSCC.output_Output_Tables();
          
          cout << "\n\n --==   Construct Output Tables  ==--";
          Constructs.output_Output_Tables_I();
     } 
     
     //Outputs the output tables for a given construct.
     void output_Construct_Output_Tables()
     {
          Constructs.output_Output_Tables();
     }
     
     //Outputs the output tables for a given construct.
     void output_Construct_Output_Tables_F()
     {
          Constructs.output_Output_Tables_F();
     }
      
     //Outputs the treetop tables.
     void output_Treetop_Tables()
     {
          SSCC.tbl_Treetops.output_I();
          Constructs.tbl_Treetops.output_I();
     }
     
     //Outputs the SSCC tables.
     void output_SSCC_Tables()
     {
          tbl_Treetops.output_I();
          tbl_Charges.output_I();
          tbl_Positions.output_I();
     }
     
     
     //Outputs the output tables for the assembly
     void output_Assembly_Outputs_L(int p_Type = -1)
     {
          if (p_Type == -1)
          {
               tbl_Output_Pattern_L.output_I();
               tbl_Output_Charges_L.output_I();
               tbl_Output_Number_Of_Patterns_L.output_I();
               tbl_Output_Position_L.output_I();
               tbl_Output_RC_L.output_F();
               tbl_Output_Treetops_L.output_I();
          }
          if (p_Type == 0)
          {
               tbl_Output_Pattern_L.output_C();
               tbl_Output_Charges_L.output_C();
               tbl_Output_Number_Of_Patterns_L.output_C();
               tbl_Output_Position_L.output_C();
               tbl_Output_RC_L.output_C();
               tbl_Output_Treetops_L.output_C();
          }
          if (p_Type == 1)
          {
               tbl_Output_Pattern_L.output_F();
               tbl_Output_Charges_L.output_F();
               tbl_Output_Number_Of_Patterns_L.output_F();
               tbl_Output_Position_L.output_F();
               tbl_Output_RC_L.output_F();
               tbl_Output_Treetops_L.output_F();
          }
          if (p_Type == 2)
          {
               tbl_Output_Pattern_L.output_I();
               tbl_Output_Charges_L.output_I();
               tbl_Output_Number_Of_Patterns_L.output_I();
               tbl_Output_Position_L.output_I();
               tbl_Output_RC_L.output_I();
               tbl_Output_Treetops_L.output_I();
          }
     }
     
     //Outputs the output tables for the assembly
     void output_Assembly_Outputs_R(int p_Type = -1)
     {
          if (p_Type == -1)
          {
               tbl_Output_Pattern_R.output_I();
               tbl_Output_Charges_R.output_I();
               tbl_Output_Number_Of_Patterns_R.output_I();
               tbl_Output_Position_R.output_I();
               tbl_Output_RC_R.output_F();
               tbl_Output_Treetops_R.output_I();
          }
          if (p_Type == 0)
          {
               tbl_Output_Pattern_R.output_C();
               tbl_Output_Charges_R.output_C();
               tbl_Output_Number_Of_Patterns_R.output_C();
               tbl_Output_Position_R.output_C();
               tbl_Output_RC_R.output_C();
               tbl_Output_Treetops_R.output_C();
          }
          if (p_Type == 1)
          {
               tbl_Output_Pattern_R.output_F();
               tbl_Output_Charges_R.output_F();
               tbl_Output_Number_Of_Patterns_R.output_F();
               tbl_Output_Position_R.output_F();
               tbl_Output_RC_R.output_F();
               tbl_Output_Treetops_R.output_F();
          }
          if (p_Type == 2)
          {
               tbl_Output_Pattern_R.output_I();
               tbl_Output_Charges_R.output_I();
               tbl_Output_Number_Of_Patterns_R.output_I();
               tbl_Output_Position_R.output_I();
               tbl_Output_RC_R.output_I();
               tbl_Output_Treetops_R.output_I();
          }
     }
     
     //Outputs the vision tables.
     void output_Vision()
     {
          tbl_Vision_Output.output_F();
          tbl_Vision_Output_Charge.output_I();
     }
     
     ////==---------------------------------+
     //==--   SAVING AND LOADING FUNCTIONS
     ////==---------------------------------+
     
     //Saves the construct.
     void Save(string p_Name, string p_Dir = "NT3.Save")
     {
          cout << "\n\n\t Saving SSCCA " << Name << "..........";
          
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
          
          //Save the SSCC.
          SSCC.Save("SSCC", tmp_Dir);
          
          //Save the construct names in the asb linker file.
          tmp_Construct_Name = Constructs.Name;
          SF << "\n" << tmp_Construct_Name;
          SF.close();
          
          //Save the constructs.
          Constructs.Save(tmp_Construct_Name, tmp_Dir);
     } 
     
     //Loads a construct.     
     void Load(string p_Name, string p_Dir = "NT3.Save")
     {
          cout << "\n\n\t Loading SSCCA " << p_Dir << "/" << p_Name << "..........";
          
          //Create the save file and directory.
          ifstream SF;
          string tmp_Dir = p_Dir + "\\" + p_Name + "\\";
          string tmp_FName = tmp_Dir + "Assembly.dat";
          string tmp_Construct_Name = "";
          
          SF.open(tmp_FName);
          
          if (!SF.is_open()){ cout << "ACCESS DENIED"; return; } else { cout << "ACCESS GRANTED"; }
          
          //--  Save settings
          
          cout << "\n\t Input File Secure, Loading Settings..........";
          
          SF >> Name;
          set_Name(Name);
          
          cout << "DONE";
          
          
          //Load the SSCC.
          SSCC.Load("SSCC", tmp_Dir);
          
          //Save the construct names in the asb linker file.
          SF >> tmp_Construct_Name;
          Constructs.Load(tmp_Construct_Name, tmp_Dir);
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
     
     
     
     //Returns data from a given constructs pattern output.
     
};

