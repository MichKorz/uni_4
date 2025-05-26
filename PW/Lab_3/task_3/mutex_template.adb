with Ada.Text_IO; use Ada.Text_IO;
with Ada.Numerics.Float_Random; use Ada.Numerics.Float_Random;
with Random_Seeds; use Random_Seeds;
with Ada.Real_Time; use Ada.Real_Time;

procedure Mutex_Template is

    -- Processes
    -- Dekker's Algorithm is for 2 processes
    Nr_Of_Processes : constant Integer := 2;

    Min_Steps : constant Integer := 5;
    Max_Steps : constant Integer := 10;

    Min_Delay : constant Duration := 0.01;
    Max_Delay : constant Duration := 0.05;

    -- States of a Process
    type Process_State is (
        Local_Section,
        Entry_Protocol,
        Critical_Section,
        Exit_Protocol
    );

    -- 2D Board display board
    Board_Width  : constant Integer := Nr_Of_Processes; -- Will be 2
    Board_Height : constant Integer := Process_State'Pos(Process_State'Last) + 1;

    -- Timing
    Start_Time : Time := Clock;  -- global starting time

    -- Random seeds for the tasks' random number generators
    Seeds : Seed_Array_Type(1 .. Nr_Of_Processes) := Make_Seeds(Nr_Of_Processes);


    -- Flag_Dekker(I) is true if process I wants to enter the critical section.
    Flag_Dekker   : array (0 .. 1) of Boolean := (others => False);
    pragma Volatile(Flag_Dekker);

    -- Turn_Dekker indicates whose turn it is if both want to enter.
    Turn_Dekker   : Integer range 0 .. 1   := 0; -- Process 0 has initial turn if contention
    pragma Volatile(Turn_Dekker);
    -- Types, procedures and functions

    -- Positions on the board
    type Position_Type is record
        X : Integer range 0 .. Board_Width - 1;
        Y : Integer range 0 .. Board_Height - 1;
    end record;

    -- traces of Processes
    type Trace_Type is record
        Time_Stamp : Duration;
        Id         : Integer;
        Position   : Position_Type;
        Symbol     : Character;
    end record;

    type Trace_Array_type is array (0 .. 4 * Max_Steps + 1) of Trace_Type;

    type Traces_Sequence_Type is record
        Last        : Integer := -1;
        Trace_Array : Trace_Array_type;
    end record;


    procedure Print_Trace(Trace : Trace_Type) is
        Symbol : String := (' ', Trace.Symbol);
    begin
        Put_Line(
            Duration'Image(Trace.Time_Stamp) & " " &
            Integer'Image(Trace.Id) & " " &
            Integer'Image(Trace.Position.X) & " " &
            Integer'Image(Trace.Position.Y) & " " &
            Symbol
        );
    end Print_Trace;

    procedure Print_Traces(Traces : Traces_Sequence_Type) is
    begin
        for I in 0 .. Traces.Last loop
            Print_Trace(Traces.Trace_Array(I));
        end loop;
    end Print_Traces;


    task Printer is
        entry Report(Traces : Traces_Sequence_Type);
    end Printer;

    task body Printer is
    begin
        for I in 1 .. Nr_Of_Processes loop
            accept Report(Traces : Traces_Sequence_Type) do
                Print_Traces(Traces);
            end Report;
        end loop;

        Put(
            "-1 " &
            Integer'Image(Nr_Of_Processes) & " " &
            Integer'Image(Board_Width) & " " &
            Integer'Image(Board_Height) & " "
        );
        for I in Process_State'Range loop
            Put(I'Image & ";");
        end loop;
        Put_Line("ALGORITHM=Dekker;"); -- Updated EXTRA_LABEL

    end Printer;


    type Process_Type is record
        Id       : Integer;
        Symbol   : Character;
        Position : Position_Type;
    end record;


    task type Process_Task_Type is
        entry Init(Id : Integer; Seed : Integer; Symbol : Character);
        entry Start;
    end Process_Task_Type;

    task body Process_Task_Type is
        G                    : Generator;
        Process              : Process_Type;
        Time_Stamp           : Duration;
        Nr_of_Steps_For_Task : Integer;
        Traces               : Traces_Sequence_Type;

        My_Id                : Integer; -- Current process's ID (0 or 1)
        Other_Id             : Integer; -- The other process's ID (0 or 1)

        procedure Store_Trace is
        begin
            if Traces.Last < Traces.Trace_Array'Last then
                Traces.Last := Traces.Last + 1;
                Traces.Trace_Array(Traces.Last) := (
                    Time_Stamp => Time_Stamp,
                    Id         => Process.Id,
                    Position   => Process.Position,
                    Symbol     => Process.Symbol
                );
            end if;
        end Store_Trace;

        procedure Change_State(State : Process_State) is
        begin
            Time_Stamp := To_Duration(Clock - Start_Time);
            Process.Position.Y := Process_State'Pos(State);
            Store_Trace;
        end;

    begin
        accept Init(Id : Integer; Seed : Integer; Symbol : Character) do
            Reset(G, Seed);
            Process.Id := Id;
            My_Id      := Id;          -- Process Id will be 0 or 1
            Other_Id   := 1 - My_Id;   -- The Id of the other process
            Process.Symbol := Symbol;
            Process.Position := (
                X => Id,
                Y => Process_State'Pos(LOCAL_SECTION)
            );
            Nr_of_Steps_For_Task := Min_Steps + Integer(Float(Max_Steps - Min_Steps) * Random(G));
            Time_Stamp := To_Duration(Clock - Start_Time);
            Store_Trace;
        end Init;

        accept Start do
            null;
        end Start;

        for Step in 1 .. Nr_of_Steps_For_Task loop
            Change_State(LOCAL_SECTION);
            delay Min_Delay + (Max_Delay - Min_Delay) * Duration(Random(G));

            Change_State(ENTRY_PROTOCOL);

            Flag_Dekker(My_Id) := True;  

            while Flag_Dekker(Other_Id) loop 
                if Turn_Dekker = Other_Id then 
                    Flag_Dekker(My_Id) := False;
                    while Turn_Dekker = Other_Id loop -- Wait until it's no longer their turn
                        delay Duration'Small;     -- Be polite, yield CPU
                    end loop;
                    Flag_Dekker(My_Id) := True;   -- Re-assert my interest
                else
                    delay Duration'Small; -- Be polite, yield CPU while waiting
                end if;
            end loop;

            Change_State(CRITICAL_SECTION);

            delay Min_Delay + (Max_Delay - Min_Delay) * Duration(Random(G));

            Change_State(EXIT_PROTOCOL);

            Turn_Dekker := Other_Id;      -- Give the turn to the other process
            Flag_Dekker(My_Id) := False;  -- I am no longer interested

        end loop;

        Change_State(LOCAL_SECTION);
        Printer.Report(Traces);
    end Process_Task_Type;


    Process_Tasks : array (0 .. Nr_Of_Processes - 1) of Process_Task_Type;
    Symbol_Char   : Character := 'A';

begin
    for I in Process_Tasks'Range loop
        Process_Tasks(I).Init(I, Seeds(I + 1), Symbol_Char);
        Symbol_Char := Character'Succ(Symbol_Char);
    end loop;

    for I in Process_Tasks'Range loop
        Process_Tasks(I).Start;
    end loop;

end Mutex_Template;
