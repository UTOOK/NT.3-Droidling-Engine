#define NT3INPUT 0
#define NT3OUTPUT 1
#define NT3CONSTRUCT 2



//This is the IO class.
class c_NT3_IO: public c_NT3_Base
{
public:
     
     int Dimension;
     c_Raw_Table_Row_1D * Row_1D;
     c_Raw_Table_Row_2D * Row_2D;
     
     virtual ~c_NT3_IO(){};
     
     void set_String(string p_String)
     {
          if (Dimension == 1){ Row_1D->set_String(0, p_String); }
     }
     
     string get_String()
     {
          if (Dimension == 1)
          { 
               if (Row_1D == NULL){ return "NO_DATA"; }
               if (Row_1D->Depth <= 0){ return "NO_DATA"; }
               return Row_1D->Cells[0]->get_String(); 
          }
          if (Dimension == 2)
          { 
               if (Row_2D == NULL){ return "NO_DATA"; }
               if (Row_2D->Depth <= 0){ return "NO_DATA"; }
               return "2D_Cell";// Row_2D->Cells[0]->get_String(); 
          }
          return "NO_DATA";
     }
     
     void set_Dimension(int p_Dimension)
     {
          reset();
          if (p_Dimension == 1)
          {
               Row_1D = new c_Raw_Table_Row_1D;
               Dimension = 1;
          }
          if (p_Dimension == 2)
          {
               Row_2D = new c_Raw_Table_Row_2D;
               Dimension = 2;
          }
     }
     
     void reset()
     {
          if (Row_1D != NULL){ delete Row_1D; Row_1D = NULL; }
          if (Row_2D != NULL){ delete Row_2D; Row_2D = NULL; }
          Dimension = 0;
     }
};

//Input class.
class c_NT3_Input: public c_NT3_IO
{
public:
     
     c_NT3_Input()
     {
          Row_1D = NULL;
          Row_2D = NULL;
          Dimension = 0;
     }
};

//Output class.
class c_NT3_Output: public c_NT3_IO
{
public:
     
     c_NT3_Output()
     {
          Row_1D = NULL;
          Row_2D = NULL;
          Dimension = 0;
     }
};


//This is the pipe class used to manage the connections between inputs, outputs, and constructs.
//Each pipe connects 2 things.
class c_NT3_Pipe
{
private:
     
     
public:
     
     //The ends of the pipe.
     void * From;
     void * To;
     
     //The rows that the ends connect to.
     void * From_Data;
     void * To_Data;
     
     //The dimensions of the input and output.
     int Dimension;
     
     //The types of object each end is.
     int From_Type;
     int To_Type;
     
     //The index of the objects in the registry.
     int From_Register;
     int To_Register;
     
     //Positions
     int fX;
     int fY;
     
     int tX;
     int tY;
     
     //The index that this pipe connects too
     int From_Index;
     int To_Index;
     
     c_NT3_Pipe()
     {
          From = NULL;
          To = NULL;
          From_Type = 0;
          To_Type = 0;
          
          From_Data = NULL;
          To_Data = NULL;
          
          Dimension = 1;
          
          fX = 0;
          fY = 0;
          tX = 0;
          tY = 0;
          
          From_Register = 0;
          To_Register = 0;
          
          From_Index = 0;
          To_Index = 0;
     }
     
     ~c_NT3_Pipe()
     {
          From = NULL;
          To = NULL;
     }
     
     //Create a pipe connections.
     void create_Connection(void * p_From, int p_From_Type, int p_From_Register, int p_From_Index, void * p_To, int p_To_Type, int p_To_Register, int p_To_Index)
     {
          From = p_From;
          From_Type = p_From_Type;
          From_Register = p_From_Register;
          From_Index = p_From_Index;
          To = p_To;
          To_Type = p_To_Type;
          To_Register = p_To_Register;
          To_Index = p_To_Index;
          
          if (From_Type == NT3INPUT)
          {
               fX = ((c_NT3_Input*) p_From)->X;
               fY = ((c_NT3_Input*) p_From)->Y;
               
               Dimension = ((c_NT3_Input*) p_From)->Dimension;
               
               if (Dimension == 1){ From_Data = ((c_NT3_Input*) p_From)->Row_1D; }
               if (Dimension == 2){ From_Data = ((c_NT3_Input*) p_From)->Row_2D; }
          }
          if (From_Type == NT3OUTPUT)
          {
               fX = ((c_NT3_Output*) p_From)->X;
               fY = ((c_NT3_Output*) p_From)->Y;
               
               Dimension = ((c_NT3_Output*) p_From)->Dimension;
               
               if (Dimension == 1){ From_Data = ((c_NT3_Output*) p_From)->Row_1D; }
               if (Dimension == 2){ From_Data = ((c_NT3_Output*) p_From)->Row_2D; }
          }
          if (From_Type == NT3CONSTRUCT)
          {
               fX = ((c_NT3_Base_Construct*) p_From)->X;
               fY = ((c_NT3_Base_Construct*) p_From)->Y;
               
               Dimension = ((c_NT3_Base_Construct*) p_From)->Dimension;
               
               if (Dimension == 1){ From_Data = ((c_NT3_Base_Construct*) p_From)->Row_1D; }
               if (Dimension == 2){ From_Data = ((c_NT3_Base_Construct*) p_From)->Row_2D; }
          }
          
          if (To_Type == NT3INPUT)
          {
               tX = ((c_NT3_Input*) p_To)->X;
               tY = ((c_NT3_Input*) p_To)->Y;
          }
          if (To_Type == NT3OUTPUT)
          {
               tX = ((c_NT3_Output*) p_To)->X;
               tY = ((c_NT3_Output*) p_To)->Y;
          }
          if (To_Type == NT3CONSTRUCT)
          {
               tX = ((c_NT3_Base_Construct*) p_To)->X;
               tY = ((c_NT3_Base_Construct*) p_To)->Y;
          }
     }
     
     //Appends cells from the input row to the output row.
     void append()
     {
          //Get the row to push too.
          
          //Append the row.
     }
     
     //Resets the output index and copies the input index to it.
     void overwrite()
     {
          //Get the row to copy from.
          
          //Get the row to push too.
          
          //Delete the push row.
          
          //Copy to the input to the push row.
     }
};

//This class is the MSC for working with multiple inputs.
class c_NT3_SANDBOX
{
private:
     
     static int stat_MAX_CONSTRUCTS;
     
     //The Discharging_Buffers. One for each input.
     c_NT3_Data_Buffer * Discharging_Buffers;
     
     string Name;
     
public:
     
     //The names of the constructs in the assembly.
     string Construct_Names[1000];
     
     //Dimension of construct.
     int Construct_Types[1000];
     
     //The constructs in the assembly.
     c_NT3_Base_Construct * Constructs[1000];
     
     //The current number of constructs.
     int Construct_Count;
     
     //The positions of the constructs.
     COORD Construct_Positions[1000];
     
     //Whether or not a construct had outputs.
     int Construct_Has_Output[1000];
     
     //The data pipes.
     c_NT3_Pipe Data_Pipes[10000];
     
     //The current number of data pipes.
     int Data_Pipe_Count;
     
     //Inputs.
     c_NT3_Input Inputs[1000];
     
     //The current number of inputs.
     int Input_Count;
     
     //The current input types.
     int Input_Dimensions[1000];
     
     //Outputs.
     c_NT3_Output Outputs[1000];
     
     //The current number of outputs.
     int Output_Count;
     
     //The output dimensions.
     int Output_Dimensions[1000];
     
     //The Register, tracks every input, construct, output, and any other objects with an ID. Used for referencing the objects a pipe attaches too.
     int Register[10000];
     int Register_Type[10000];
     int Register_Current_Count;
     
     c_NT3_SANDBOX()
     {
          Name = "Generic_Sandbox_Assembly";
          
          for (int cou_Index=0;cou_Index<1000;cou_Index++)
          {
               Construct_Names[cou_Index] = ("Construct_" + int2str(cou_Index));
               Constructs[cou_Index] = NULL;
               Construct_Positions[cou_Index].X = 0;
               Construct_Positions[cou_Index].Y = 0;
               Construct_Has_Output[cou_Index] = 0;
               
               Input_Dimensions[cou_Index] = 0;
               Output_Dimensions[cou_Index] = 0;
               
               Inputs[cou_Index].reset();
               Outputs[cou_Index].reset();
          }
          
          for (int cou_Index=0;cou_Index<10000;cou_Index++)
          {
               Register[cou_Index] = 0;
               Register_Type[cou_Index] = 0;
          }
          
          Construct_Count = 0;
          Data_Pipe_Count = 0;
          Input_Count = 0;
          Output_Count = 0;
          Register_Current_Count = 0;
     }
     
     ~c_NT3_SANDBOX()
     {
          cout << "\n ~c_NT3_SANDBOX " << Name << " " << this << ".........."; cout.flush();
          
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               delete Constructs[cou_Index];
               Constructs[cou_Index] = NULL;
          }
          cout << "\n ~~~c_NT3_SANDBOX " << Name << " " << this << ".........."; cout.flush();
     }
     
     //Sets the name of the assembly.
     void set_Name(string p_Name)
     {
          Name = p_Name;
          
          string tmp_Construct_Name = "";
          for (int cou_Index=0;cou_Index<1000;cou_Index++)
          {
               //tmp_Construct_Name = Name + "_Construct_" + int2str(cou_Index);
               //Constructs[cou_Index]->set_Name(tmp_Construct_Name);
               //Construct_Names[cou_Index] = tmp_Construct_Name;
          }
     }
     
     //Create new construct.
     void create_Construct(string p_Name, int p_Type = 1)
     {
          if (p_Type == 0)
          {
               Constructs[Construct_Count] = new c_NT3_Construct_NonStrict;
               Construct_Positions[Construct_Count].X = 40;
               Constructs[Construct_Count]->X = 40;
          }
          if (p_Type == 1)
          {
               Constructs[Construct_Count] = new c_NT3_Construct_1D;
               Construct_Positions[Construct_Count].X = 10;
               Constructs[Construct_Count]->X = 10;
          }
          if (p_Type == 2)
          {
               Constructs[Construct_Count] = new c_NT3_Construct_2D;
               Construct_Positions[Construct_Count].X = 10;
               Constructs[Construct_Count]->X = 10;
          }
          if (p_Type == 3)
          {
               //Constructs[Construct_Count] = new c_NT3_Construct_3D;
               Construct_Positions[Construct_Count].X = 10;
               Constructs[Construct_Count]->X = 10;
          }
          Construct_Names[Construct_Count] = p_Name;
          Construct_Types[Construct_Count] = p_Type;
          Construct_Positions[Construct_Count].Y = (Construct_Count * 15);
          Constructs[Construct_Count]->Y = (Construct_Count * 15);
          
          Register[Register_Current_Count] = Construct_Count;
          Register_Type[Register_Current_Count] = NT3CONSTRUCT; //2 is for a construct.
          Register_Current_Count++;
          
          //All constructs, inputs, and outputs are held in a bin tree with universal IDs
          
          Construct_Count++;
     }
     
     //Creates a new input and adds it to the input table.
     //Returns the index of the new input.
     int create_Input(int p_Dimension = 1)
     {
          if (p_Dimension == 1)
          {
               Input_Dimensions[Input_Count] = 1;
               Inputs[Input_Count].X = 0;
               Inputs[Input_Count].Y = (Input_Count * 15);
               //Create 1D output.
          }
          if (p_Dimension == 2)
          {
               Input_Dimensions[Input_Count] = 2;
               Inputs[Input_Count].X = 0;
               Inputs[Input_Count].Y = (Input_Count * 15);
               //Create 2D output.
          }
          
          Register[Register_Current_Count] = Input_Count;
          Register_Type[Register_Current_Count] = NT3INPUT; //2 is for a construct.
          Register_Current_Count++;
          
          Inputs[Input_Count].set_Dimension(p_Dimension);
          
          Input_Count++;
          return Input_Count - 1;
     }
     
     //Creates a new output and adds it to the output table.
     //Returns the index of the new output.
     int create_Output(int p_Dimension = 1)
     {
          if (p_Dimension == 1)
          {
               Output_Dimensions[Output_Count] = 1;
               Outputs[Output_Count].X = 75;
               Outputs[Output_Count].Y = (Output_Count * 15);
               //Create 1D output.
          }
          if (p_Dimension == 2)
          {
               Output_Dimensions[Output_Count] = 2;
               Outputs[Output_Count].X = 75;
               Outputs[Output_Count].Y = (Output_Count * 15);
               //Create 2D output.
          }
          
          Register[Register_Current_Count] = Output_Count;
          Register_Type[Register_Current_Count] = NT3OUTPUT; //2 is for a construct.
          Register_Current_Count++;
          
          Outputs[Output_Count].set_Dimension(p_Dimension);
          
          Output_Count++;
          return Output_Count - 1;
     }
     
     //Creates a connection.
     int create_Connection(int p_From, int p_From_Index, int p_To, int p_To_Index)
     {
          void * tmp_From = NULL;
          void * tmp_To = NULL;
          
          //cout << "\n\n p_From->" << p_From << " p_To->" << p_To;
          //cout << "\n " << NT3INPUT << " " << NT3OUTPUT << " " << NT3CONSTRUCT;
          //cout << "\n " << Register_Type[p_From] << " " << Register
          
          //Get the from connection.
          if (Register_Type[p_From] == NT3INPUT)
          {
               tmp_From = &Inputs[Register[p_From]];
          }
          if (Register_Type[p_From] == NT3OUTPUT)
          {
               tmp_From = &Outputs[Register[p_From]];
          }
          if (Register_Type[p_From] == NT3CONSTRUCT)
          {
               tmp_From = Constructs[Register[p_From]];
          }
          
          //Get the to connection.
          if (Register_Type[p_To] == NT3INPUT)
          {
               tmp_To = &Inputs[Register[p_To]];
          }
          if (Register_Type[p_To] == NT3OUTPUT)
          {
               tmp_To = &Outputs[Register[p_To]];
          }
          if (Register_Type[p_To] == NT3CONSTRUCT)
          {
               tmp_To = Constructs[Register[p_To]];
          }
          
          //Create the connections.
          Data_Pipes[Data_Pipe_Count].create_Connection(tmp_From, Register_Type[p_From], p_From, p_From_Index, tmp_To, Register_Type[p_To], p_To, p_To_Index);
          
          Data_Pipe_Count++;
          
          return 1;
     }
     
     
     //Appends cells from the first index to the row in the second index.
     void pipe_Append(int p_Pipe)
     {
          //Check for pipe.
          if (p_Pipe >= Data_Pipe_Count){ return; }
          
          Data_Pipes[p_Pipe].append();
     }
     
     //Resets the output index and copies the input index to it.
     void pipe_Overwrite(int p_Pipe)
     {
          //Check for pipe.
          if (p_Pipe >= Data_Pipe_Count){ return; }
          
          Data_Pipes[p_Pipe].overwrite();
     }
     
     //Outputs the constructs to the console, bretty basic.
     void output_Raw()
     {
          system("CLS");
          for (int cou_Index=0;cou_Index<Data_Pipe_Count;cou_Index++)
          {
               console_Draw_Line(Data_Pipes[cou_Index].fX, Data_Pipes[cou_Index].fY, Data_Pipes[cou_Index].tX, Data_Pipes[cou_Index].tY, (7 + cou_Index));
          }
          for (int cou_Index=0;cou_Index<Input_Count;cou_Index++)
          {
               xy(0, (cou_Index * 15));
               cout << "<-";
               oint(0, 14, cou_Index);
               cout << " ";
               oint(0, 7, Inputs[cou_Index].Dimension);
               cout << "->";
          }
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               xy(Construct_Positions[cou_Index].X, Construct_Positions[cou_Index].Y);
               //oint(0, 7, cou_Index); cout << "("; ostr(0, 8, "\""); ostr(0, 7, Construct_Names[cou_Index]); ostr(0, 8, "\""); cout << " "; oint(0, 13, Construct_Types[cou_Index]); cout << ")";
               
               cout << "(";  oint(0, 7, cou_Index); cout << " "; oint(0, 13, Construct_Types[cou_Index]); cout << ")";
          }
          for (int cou_Index=0;cou_Index<Output_Count;cou_Index++)
          {
               xy(Outputs[cou_Index].X, Outputs[cou_Index].Y);
               cout << "<-";
               oint(0, 15, cou_Index);
               cout << " ";
               oint(0, 8, Outputs[cou_Index].Dimension);
               cout << "->";
          }
          xy(0, 75);
          output_Registry();
          cout << "\n";
          output_Pipe_Data();
     }
     
     void output_Registry()
     {
          cout << "\n Registry->";
          for (int cou_Index=0;cou_Index<Register_Current_Count;cou_Index++)
          {
               cout << "\n";
               oint(0, 8, cou_Index);
               
               cout << " "; oint(0, (7 + Register_Type[cou_Index]), Register[cou_Index]);
               cout << " "; oint(0, (13 + Register_Type[cou_Index]), Register_Type[cou_Index]);
               
               if (Register_Type[cou_Index] == NT3INPUT)
               {
                    ostr(0, (13 + Register_Type[cou_Index]), " Input ");
               }
               if (Register_Type[cou_Index] == NT3OUTPUT)
               {
                    ostr(0, (13 + Register_Type[cou_Index]), " Output ");
               }
               if (Register_Type[cou_Index] == NT3CONSTRUCT)
               {
                    ostr(0, (13 + Register_Type[cou_Index]), " Construct "); ostr(0, 8, Construct_Names[Register[cou_Index]]);
               }
          }
     }
     
     void output_Pipe_Data()
     {
          int tmp_R = 0;
          
          cout << "\n Data_Pipes->";
          for (int cou_Index=0;cou_Index<Data_Pipe_Count;cou_Index++)
          {
               cout << "\n";
               oint(0, 8, cou_Index);
               
               
               //From
               tmp_R = Data_Pipes[cou_Index].From_Register;
               
               cout << " From->"; oint(0, (7 + Register_Type[tmp_R]), Register[tmp_R]);
               cout << " "; oint(0, (13 + Register_Type[tmp_R]), Register_Type[tmp_R]);
               cout << " "; oint(0, 8, Data_Pipes[cou_Index].From_Index);
               
               if (Register_Type[tmp_R] == NT3INPUT)
               {
                    ostr(0, (13 + Register_Type[tmp_R]), " Input ");
               }
               if (Register_Type[tmp_R] == NT3OUTPUT)
               {
                    ostr(0, (13 + Register_Type[tmp_R]), " Output ");
               }
               if (Register_Type[tmp_R] == NT3CONSTRUCT)
               {
                    ostr(0, (13 + Register_Type[tmp_R]), " Construct "); ostr(0, 8, Construct_Names[Register[tmp_R]]);
               }
               
               //To
               tmp_R = Data_Pipes[cou_Index].To_Register;
               
               cout << " To->"; oint(0, (7 + Register_Type[tmp_R]), Register[tmp_R]);
               cout << " "; oint(0, (13 + Register_Type[tmp_R]), Register_Type[tmp_R]);
               cout << " "; oint(0, 8, Data_Pipes[cou_Index].To_Index);
               
               if (Register_Type[tmp_R] == NT3INPUT)
               {
                    ostr(0, (13 + Register_Type[tmp_R]), " Input ");
               }
               if (Register_Type[tmp_R] == NT3OUTPUT)
               {
                    ostr(0, (13 + Register_Type[tmp_R]), " Output ");
               }
               if (Register_Type[tmp_R] == NT3CONSTRUCT)
               {
                    ostr(0, (13 + Register_Type[tmp_R]), " Construct "); ostr(0, 8, Construct_Names[Register[tmp_R]]);
               }
          }
     }
     
     //Outputs the input information.
     void output_Input_Data()
     {
          cout << "\n Inputs->" << Input_Count;
          for (int cou_Index=0;cou_Index<Input_Count;cou_Index++)
          {
               cout << "\n " << Inputs[cou_Index].get_String();
          }
     }
     
     //Outputs the input information.
     void output_Output_Data()
     {
          cout << "\n Outputs->" << Output_Count;
          for (int cou_Index=0;cou_Index<Output_Count;cou_Index++)
          {
               cout << "\n " << Outputs[cou_Index].get_String();
          }
     }
     
     //Outputs the output information.
     
     
     //==---    IO
     
     //Sets a given input to the given string.
     void set_Input_String(int p_Input, string p_Data)
     {
          Inputs[p_Input].set_String(p_Data);
          cout << "\n Inputs[" << p_Input << "].set_String(0, " << p_Data << "); ";
          output_Input_Data();
     }
     
     
     //Saves the sandbox.
     void Save(string p_DIR, string p_FName = "NONAME")
     {
          if (p_FName == "NONAME"){ p_FName = Name; }
          string tmp_FName = p_DIR + p_FName + ".sand";
          
          ofstream SF;
          SF.open(tmp_FName, ios::trunc);
          if (!SF.is_open()){ SF.close(); cout << "\n\n ERROR CANNOT OPEN SAVE FILE " << tmp_FName; return; }
          
          SF << "Register " << Register_Current_Count;
          for (int cou_Index=0;cou_Index<Register_Current_Count;cou_Index++)
          {
               SF << "\n";
               SF << Register_Type[cou_Index];
               if (Register_Type[cou_Index] == NT3INPUT)
               {
                    SF << " NT3INPUT";
                    
                    //Save the dimension.
                    SF << " D " << Inputs[Register[cou_Index]].Dimension;
                    
                    //Coordinates.
                    SF << " X " << Inputs[Register[cou_Index]].X;
                    SF << " Y " << Inputs[Register[cou_Index]].Y;
               }
               if (Register_Type[cou_Index] == NT3OUTPUT)
               {
                    SF << " NT3OUTPUT";
                    
                    //Save the dimension.
                    SF << " D " << Outputs[Register[cou_Index]].Dimension;
                    
                    //Coordinates.
                    SF << " X " << Outputs[Register[cou_Index]].X;
                    SF << " Y " << Outputs[Register[cou_Index]].Y;
               }
               if (Register_Type[cou_Index] == NT3CONSTRUCT)
               {
                    SF << " NT3CONSTRUCT";
                    
                    //Save the name.
                    SF << " " << Construct_Names[Register[cou_Index]];
                    
                    //Coordinates.
                    SF << " X " << Constructs[Register[cou_Index]]->X;
                    SF << " Y " << Constructs[Register[cou_Index]]->Y;
                    
                    //Save the type.
                    SF << " T " << Construct_Types[Register[cou_Index]];
               }
          }
          
          SF << "\nData_Pipes " << Data_Pipe_Count;
          for (int cou_Index=0;cou_Index<Data_Pipe_Count;cou_Index++)
          {
               SF << "\n";
               
               //From
               SF << "FR " << Data_Pipes[cou_Index].From_Register;
               SF << " FI " << Data_Pipes[cou_Index].From_Index;
               SF << " TR " << Data_Pipes[cou_Index].To_Register;
               SF << " TI " << Data_Pipes[cou_Index].To_Index;
               
          }
          
          SF.close();
          
          tmp_FName = p_DIR + "ConIndex.con";
          SF.open(tmp_FName, ios::trunc);
          
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               if (cou_Index != 0){ SF << "\n"; }
               SF << Construct_Names[cou_Index] + ".con";
               
               tmp_FName = Construct_Names[cou_Index] + ".con";
               Constructs[cou_Index]->Save(tmp_FName, p_DIR);
          }
          SF.close();
     }
     
     //Loads the sandbox.
     void Load(string p_DIR, string p_FName)
     {
          string tmp_FName = p_DIR + p_FName + ".sand";
          
          ifstream SF;
          SF.open(tmp_FName);
          if (!SF.is_open()){ SF.close(); cout << "\n\n ERROR CANNOT OPEN SAVE FILE " << tmp_FName; return; }
          
          string tmp_Input;
          int tmp_Dimension;
          int tmp_X;
          int tmp_Y;
          int tmp_Type;
          string tmp_Input_Name;
          int tmp_Register_Count;
          
          int tmp_Data_Pipe_Count;
          int tmp_Data_Pipe_From_Register;
          int tmp_Data_Pipe_From_Index;
          int tmp_Data_Pipe_To_Register;
          int tmp_Data_Pipe_To_Index;
          
          //Get the Register string and the number of registers.
          SF >> tmp_Input;
          SF >> tmp_Register_Count;
          /*7*/cout << "\n " << tmp_Input << " (" << tmp_Register_Count << ")";
          
          //Loop through the registers reading them into the register based on the type they are.
          for (int cou_Index=0;cou_Index<tmp_Register_Count;cou_Index++)
          {
               SF >> Register_Type[cou_Index];
               SF >> tmp_Input;
               /*7*/cout << "\n " << cou_Index << " " << tmp_Register_Count << " " << tmp_Input << " " << Register_Type[cou_Index];
               
               if (Register_Type[cou_Index] == NT3INPUT)
               {
                    //Save the dimension.
                    SF >> tmp_Input;
                    SF >> tmp_Dimension;
                    
                    //Coordinates.
                    SF >> tmp_Input;
                    SF >> tmp_X;
                    
                    SF >> tmp_Input;
                    SF >> tmp_Y;
                    
                    /*7*/cout << " INPUT D " << tmp_Dimension << " X " << tmp_X << " Y " << tmp_Y;
                    
                    create_Input(tmp_Dimension);
                    Inputs[Input_Count - 1].X = tmp_X;
                    Inputs[Input_Count - 1].Y = tmp_Y;
               }
               if (Register_Type[cou_Index] == NT3OUTPUT)
               {
                    //Save the dimension.
                    SF >> tmp_Input;
                    SF >> tmp_Dimension;
                    
                    //Coordinates.
                    SF >> tmp_Input;
                    SF >> tmp_X;
                    
                    SF >> tmp_Input;
                    SF >> tmp_Y;
                    
                    /*7*/cout << " OUTPUT D " << tmp_Dimension << " X " << tmp_X << " Y " << tmp_Y;
                    
                    create_Output(tmp_Dimension);
                    Outputs[Output_Count - 1].X = tmp_X;
                    Outputs[Output_Count - 1].Y = tmp_Y;
               }
               if (Register_Type[cou_Index] == NT3CONSTRUCT)
               {
                    //Save the name.
                    SF >> tmp_Input_Name;
                    
                    //Coordinates.
                    SF >> tmp_Input;
                    SF >> tmp_X;
                    
                    SF >> tmp_Input;
                    SF >> tmp_Y;
                    
                    SF >> tmp_Input;
                    SF >> tmp_Type;
                    
                    /*7*/cout << " CONSTRUCT " << tmp_Input_Name << " X " << tmp_X << " Y " << tmp_Y << " T " << tmp_Type;
                    
                    create_Construct(tmp_Input_Name, tmp_Type);
                    Constructs[Construct_Count - 1]->X = tmp_X;
                    Constructs[Construct_Count - 1]->Y = tmp_Y;
               }
          }
          
          SF >> tmp_Input;
          SF >> tmp_Data_Pipe_Count; 
          cout << "\n Data_Pipes " << tmp_Data_Pipe_Count;
          for (int cou_Index=0;cou_Index<tmp_Data_Pipe_Count;cou_Index++)
          {
               //From
               SF >> tmp_Input;
               SF >> tmp_Data_Pipe_From_Register;
               SF >> tmp_Input;
               SF >> tmp_Data_Pipe_From_Index;
               
               //To
               SF >> tmp_Input;
               SF >> tmp_Data_Pipe_To_Register;
               SF >> tmp_Input;
               SF >> tmp_Data_Pipe_To_Index;
               
               /*7*/cout << "\n From->" << tmp_Data_Pipe_From_Register << "." << tmp_Data_Pipe_From_Index << " To->" << tmp_Data_Pipe_To_Register << "." << tmp_Data_Pipe_To_Index;
               create_Connection(tmp_Data_Pipe_From_Register, tmp_Data_Pipe_From_Index, tmp_Data_Pipe_To_Register, tmp_Data_Pipe_To_Index);
          }
          
          SF.close();
          
          for (int cou_Index=0;cou_Index<Construct_Count;cou_Index++)
          {
               tmp_FName = Construct_Names[cou_Index] + ".con";
               Constructs[cou_Index]->Load(tmp_FName, p_DIR);
          }
     }
     
};

int c_NT3_SANDBOX::stat_MAX_CONSTRUCTS = 1000;