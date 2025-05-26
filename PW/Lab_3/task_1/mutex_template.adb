with Ada.Text_IO; use Ada.Text_IO;
with Ada.Numerics.Float_Random; use Ada.Numerics.Float_Random;
with Random_Seeds; use Random_Seeds;
with Ada.Real_Time; use Ada.Real_Time;

procedure Mutex_Template is

    -- Processes
    Nr_Of_Processes : constant Integer := 20;

    Min_Steps : constant Integer := 10; 
    Max_Steps : constant Integer := 40;

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
    Board_Width  : constant Integer := Nr_Of_Processes;
    Board_Height : constant Integer := Process_State'Pos(Process_State'Last) + 1;

    -- Timing
    Start_Time : Time := Clock;  -- global starting time

    -- Random seeds for the tasks' random number generators
    Seeds : Seed_Array_Type(1 .. Nr_Of_Processes) := Make_Seeds(Nr_Of_Processes);


    -- Choosing(I) is true if process I is picking a number.
    Choosing : array (0 .. Nr_Of_Processes - 1) of Boolean := (others => False);
    pragma Volatile(Choosing); -- Ensure reads/writes are not optimized away across tasks

    -- Number(I) is the ticket number for process I. 0 means no ticket.
    Number   : array (0 .. Nr_Of_Processes - 1) of Integer := (others => 0);
    pragma Volatile(Number);   -- Ensure reads/writes are not optimized away across tasks


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

    -- CORRECTED ARRAY SIZE DEFINITION HERE:
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
            Symbol -- print as string to avoid: '
        );
    end Print_Trace;

    procedure Print_Traces(Traces : Traces_Sequence_Type) is
    begin
        for I in 0 .. Traces.Last loop
            Print_Trace(Traces.Trace_Array(I));
        end loop;
    end Print_Traces;


    -- task Printer collects and prints reports of traces and the line with the parameters
    task Printer is
        entry Report(Traces : Traces_Sequence_Type);
    end Printer;

    task body Printer is
    begin
        -- Collect and print the traces
        for I in 1 .. Nr_Of_Processes loop
            accept Report(Traces : Traces_Sequence_Type) do
                Print_Traces(Traces);
            end Report;
        end loop;

        -- Print the line with the parameters needed for display script:
        Put(
            "-1 " &
            Integer'Image(Nr_Of_Processes) & " " &
            Integer'Image(Board_Width) & " " &
            Integer'Image(Board_Height) & " "
        );
        for I in Process_State'Range loop
            Put(I'Image & ";");
        end loop;
        Put_Line("ALGORITHM=Bakery;");

    end Printer;


    -- Processes
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
        G           : Generator;
        Process     : Process_Type;
        Time_Stamp  : Duration;
        Nr_of_Steps_For_Task : Integer;
        Traces      : Traces_Sequence_Type;

        My_Id       : Integer;

        procedure Store_Trace is
        begin
            if Traces.Last < Traces.Trace_Array'Last then -- Bounds check
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
            Time_Stamp := To_Duration(Clock - Start_Time); -- reads global clock
            Process.Position.Y := Process_State'Pos(State);
            Store_Trace;
        end;


    begin
        accept Init(Id : Integer; Seed : Integer; Symbol : Character) do
            Reset(G, Seed);
            Process.Id := Id;
            My_Id      := Id; -- Store ID for Bakery algorithm
            Process.Symbol := Symbol;
            -- Initial position
            Process.Position := (
                X => Id,
                Y => Process_State'Pos(LOCAL_SECTION)
            );
            -- Number of steps to be made by the Process
            Nr_of_Steps_For_Task := Min_Steps + Integer(Float(Max_Steps - Min_Steps) * Random(G));

            -- Time_Stamp of initialization
            Time_Stamp := To_Duration(Clock - Start_Time); -- reads global clock
            Store_Trace; -- store starting position
        end Init;

        -- wait for initialisations of the remaining tasks:
        accept Start do
            null;
        end Start;

        for Step in 1 .. Nr_of_Steps_For_Task loop -- Use the task-specific number of steps
            -- LOCAL_SECTION - start
            Change_State(LOCAL_SECTION); -- Ensure state is marked at start of loop iteration
            delay Min_Delay + (Max_Delay - Min_Delay) * Duration(Random(G));
            -- LOCAL_SECTION - end

            Change_State(ENTRY_PROTOCOL); -- starting ENTRY_PROTOCOL
            Choosing(My_Id) := True;

            declare
                Max_Ticket_Val : Integer := 0; -- Renamed to avoid potential conflict with Max_Ticket if it were a global
            begin
                for K in 0 .. Nr_Of_Processes - 1 loop
                    if Number(K) > Max_Ticket_Val then
                        Max_Ticket_Val := Number(K);
                    end if;
                end loop;
                Number(My_Id) := Max_Ticket_Val + 1;
            end;

            Choosing(My_Id) := False;

            for J in 0 .. Nr_Of_Processes - 1 loop
                if J /= My_Id then -- Only check other processes
                    -- Wait if process J is currently choosing its number
                    while Choosing(J) loop
                        delay Duration'Small;
                    end loop;

                    while (Number(J) /= 0) and then
                          ((Number(J) < Number(My_Id)) or
                           (Number(J) = Number(My_Id) and then J < My_Id))
                    loop
                        delay Duration'Small; -- Small delay
                    end loop;
                end if;
            end loop;

            Change_State(CRITICAL_SECTION); -- starting CRITICAL_SECTION

            -- CRITICAL_SECTION - start
            delay Min_Delay + (Max_Delay - Min_Delay) * Duration(Random(G));
            -- CRITICAL_SECTION - end

            Change_State(EXIT_PROTOCOL); -- starting EXIT_PROTOCOL
            Number(My_Id) := 0;
        end loop;

        -- After loop, explicitly set to Local_Section before reporting
        Change_State(LOCAL_SECTION);

        Printer.Report(Traces);
    end Process_Task_Type;


    -- local for main task
    Process_Tasks : array (0 .. Nr_Of_Processes - 1) of Process_Task_Type;
    Symbol_Char   : Character := 'A';

begin
    -- init traveler tasks
    for I in Process_Tasks'Range loop
        Process_Tasks(I).Init(I, Seeds(I + 1), Symbol_Char);
        Symbol_Char := Character'Succ(Symbol_Char);
    end loop;

    -- start traveler tasks
    for I in Process_Tasks'Range loop
        Process_Tasks(I).Start;
    end loop;

end Mutex_Template;
