with Ada.Text_IO; use Ada.Text_IO;
with Ada.Numerics.Float_Random; use Ada.Numerics.Float_Random;
with Random_Seeds; use Random_Seeds;
with Ada.Real_Time; use Ada.Real_Time;

procedure Mutex_Template is


-- Processes 

  Nr_Of_Processes : constant Integer := 15;

  Min_Steps : constant Integer := 50 ;
  Max_Steps : constant Integer := 100 ;

  Min_Delay : constant Duration := 0.01;
  Max_Delay : constant Duration := 0.05;

-- States of a Process 

  type Process_State is (
    Local_Section,
    Entry_Protocol_1,
    Entry_Protocol_2,
    Entry_Protocol_3,
    Entry_Protocol_4,
    Critical_Section,
    Exit_Protocol
    );
    
  -- The Flags array is the core of Szymański's algorithm. Each process has a flag
  -- that represents its current state of contention for the critical section.
  -- The possible flag values (0-4) correspond to different stages of the protocol:
  -- 0: Not interested in the critical section (in Local_Section).
  -- 1: Interested, standing at the "doorway."
  -- 2: Actively waiting for other processes.
  -- 3: Passed the first wait condition.
  -- 4: Ready to enter the critical section.
  Flags : array(0 .. Nr_Of_Processes - 1) of Integer range 0..4 := (others => 0);

-- 2D Board display board

  Board_Width  : constant Integer := Nr_Of_Processes;
  Board_Height : constant Integer := Process_State'Pos( Process_State'Last ) + 1;

-- Timing

  Start_Time : Time := Clock;  -- global startnig time

-- Random seeds for the tasks' random number generators
  
  Seeds : Seed_Array_Type( 1..Nr_Of_Processes ) := Make_Seeds( Nr_Of_Processes );

-- Types, procedures and functions

  -- Postitions on the board
  type Position_Type is record     
    X: Integer range 0 .. Board_Width - 1; 
    Y: Integer range 0 .. Board_Height - 1; 
  end record;       

  -- traces of Processes
  type Trace_Type is record         
    Time_Stamp:  Duration;         
    Id : Integer;
    Position: Position_Type;     
    Symbol: Character;         
  end record;         

  type Trace_Array_type is  array(0 .. Max_Steps) of Trace_Type;

  type Traces_Sequence_Type is record
    Last: Integer := -1;
    Trace_Array: Trace_Array_type ;
  end record; 


  procedure Print_Trace( Trace : Trace_Type ) is
    Symbol : String := ( ' ', Trace.Symbol );
  begin
    Put_Line(
         Duration'Image( Trace.Time_Stamp ) & " " &
         Integer'Image( Trace.Id ) & " " &
         Integer'Image( Trace.Position.X ) & " " &
         Integer'Image( Trace.Position.Y ) & " " &
         ( ' ', Trace.Symbol ) -- print as string to avoid: '
     );
  end Print_Trace;

  procedure Print_Traces( Traces : Traces_Sequence_Type ) is
  begin
    for I in 0 .. Traces.Last loop
      Print_Trace( Traces.Trace_Array( I ) );
    end loop;
  end Print_Traces;


  -- task Printer collects and prints reports of traces and the line with the parameters

  task Printer is
    entry Report( Traces : Traces_Sequence_Type );
  end Printer;
  
  task body Printer is 
  begin
  
    -- Collect and print the traces
    
    for I in 1 .. Nr_Of_Processes loop -- range for TESTS !!!
        accept Report( Traces : Traces_Sequence_Type ) do
         --   Put_Line("I = " & I'Image );
          Print_Traces( Traces );
        end Report;
      end loop;

    -- Prit the line with the parameters needed for display script:

    Put(
      "-1 "&
      Integer'Image( Nr_Of_Processes ) &" "&
      Integer'Image( Board_Width ) &" "&
      Integer'Image( Board_Height ) &" "      
    );
    for I in Process_State'Range loop
      Put( I'Image &";" );
    end loop;
    Put_Line("EXTRA_LABEL;"); -- Place labels with extra info here (e.g. "MAX_TICKET=...;" for Backery). 

  end Printer;


  -- Processes
  type Process_Type is record
    Id: Integer;
    Symbol: Character;
    Position: Position_Type;   
  end record;


  task type Process_Task_Type is     
    entry Init(Id: Integer; Seed: Integer; Symbol: Character);
    entry Start;
  end Process_Task_Type;   

  task body Process_Task_Type is
    G : Generator;
    Process : Process_Type;
    Time_Stamp : Duration;
    Nr_of_Steps: Integer;
    Traces: Traces_Sequence_Type; 
    Condition: Boolean;

    procedure Store_Trace is
    begin  
      Traces.Last := Traces.Last + 1;
      Traces.Trace_Array( Traces.Last ) := ( 
          Time_Stamp => Time_Stamp,
          Id => Process.Id,
          Position => Process.Position,
          Symbol => Process.Symbol
        );
    end Store_Trace;

    procedure Change_State( State: Process_State ) is
    begin
      Time_Stamp := To_Duration ( Clock - Start_Time ); -- reads global clock
      Process.Position.Y := Process_State'Pos( State );
      Store_Trace;
    end;
    

  begin
    accept Init(Id: Integer; Seed: Integer; Symbol: Character) do
      Reset(G, Seed); 
      Process.Id := Id;
      Process.Symbol := Symbol;
      -- Initial position 
      Process.Position := (
          X => Id,
          Y => Process_State'Pos( LOCAL_SECTION )
        );
      -- Number of steps to be made by the Process  
      Nr_of_Steps := Min_Steps + Integer( Float(Max_Steps - Min_Steps) * Random(G));
      -- Time_Stamp of initialization
      Time_Stamp := To_Duration ( Clock - Start_Time ); -- reads global clock
      Store_Trace; -- store starting position
    end Init;
    
    -- wait for initialisations of the remaining tasks:
    accept Start do
      null;
    end Start;

--    for Step in 0 .. Nr_of_Steps loop
    for Step in 0 .. Nr_of_Steps/7 - 1  loop  -- TEST !!!
      -- LOCAL_SECTION - start
      delay Min_Delay+(Max_Delay-Min_Delay)*Duration(Random(G));
      -- LOCAL_SECTION - end

      Change_State( ENTRY_PROTOCOL_1 ); -- starting ENTRY_PROTOCOL
      -- Szymański's Algorithm Entry Protocol Starts Here
      
      -- Step 1: Announce interest.
      -- The process sets its flag to 1, indicating it wants to enter the critical section.
      -- This is like standing in the "doorway."
      Flags(Process.Id) := 1;
      
      -- Step 2: Wait for a clear path.
      -- The process waits until no other process is in the final stages of entry (flag 3)
      -- or already in the critical section (flag 4).
      Condition := False;
      while not Condition loop
          Condition := True;
          for I in Flags'Range loop
              if Flags(I) > 2 then
                  Condition := False;
                  exit;
              end if;
          end loop;
      end loop;

      -- Step 3: Advance to the next stage.
      -- Having passed the first wait, the process sets its flag to 3.
      Flags(Process.Id) := 3;
      Change_State (Entry_Protocol_2);

      -- Step 4: Check for contention at the "doorway".
      -- The process checks if any other process is still at the initial stage (flag 1).
      for I in Flags'Range loop
          if Flags(I) = 1 then
              -- Step 4a: If there is contention, move to a waiting state.
              -- The process sets its flag to 2 and waits until no other process
              -- is in the critical section (flag 4). This prevents deadlock
              -- by allowing the other process to proceed.
              Flags(Process.Id) := 2;
              Change_State (Entry_Protocol_3);

              Condition := False;
              while not Condition loop
                  for J in Flags'Range loop -- Note: loop variable changed to J to avoid conflict
                      if Flags(J) = 4 then
                          Condition := True;
                          exit;
                      end if;
                  end loop;
              end loop;
              exit; -- Exit the outer loop after waiting
          end if;
      end loop;
           
      -- Step 5: Final approach.
      -- If there was no contention or after waiting, the process signals its
      -- imminent entry by setting its flag to 4.
      Flags(Process.Id) := 4;
      Change_State (Entry_Protocol_4);

      -- Step 6: Final priority check (ensuring fairness).
      -- The process waits for all processes with a lower ID (and thus higher priority)
      -- to finish. It checks for any flag value greater than 1, ensuring those
      -- processes that are ahead in the protocol get to enter first.
      Condition := False;
      if Process.Id /= 0 then 
          while Condition = False loop
              Condition := True;
              for I in 0..Process.Id-1 loop
                  if Flags(I) > 1 then
                      Condition := False;
                      exit;
                  end if;
              end loop;
          end loop;
      end if;
      -- End of Entry Protocol

      Change_State( CRITICAL_SECTION ); -- starting CRITICAL_SECTION

      -- CRITICAL_SECTION - start
      delay Min_Delay+(Max_Delay-Min_Delay)*Duration(Random(G));
      -- CRITICAL_SECTION - end

      Change_State( EXIT_PROTOCOL ); -- starting EXIT_PROTOCOL
      -- Szymański's Algorithm Exit Protocol Starts Here

      -- Step 7: Acknowledging other waiting processes.
      -- Before fully exiting, the process waits for any higher-ID processes that were
      -- in waiting states (2 or 3) to advance. This is a fairness mechanism
      -- to prevent starving processes with higher IDs.
      Condition := False;
      while not Condition loop
          Condition := True;
          for I in Process.Id+1..Nr_Of_Processes-1 loop
              if Flags(I) = 2 or Flags(I) = 3 then
                  Condition := False;
                  exit;
              end if;
          end loop;
      end loop;

      -- Step 8: Exit completely.
      -- The process resets its flag to 0, indicating it is no longer interested
      -- in the critical section. This signals to all other waiting processes
      -- that it has finished.
      Flags(Process.Id) := 0;
      -- End of Exit Protocol
      
      Change_State( LOCAL_SECTION ); -- starting LOCAL_SECTION     
    end loop;
    
    Printer.Report( Traces );
  end Process_Task_Type;


-- local for main task

  Process_Tasks: array (0 .. Nr_Of_Processes-1) of Process_Task_Type; -- for tests
  Symbol : Character := 'A';

begin 
  -- init tarvelers tasks
  for I in Process_Tasks'Range loop
    Process_Tasks(I).Init( I, Seeds(I+1), Symbol );
    Symbol := Character'Succ( Symbol );
  end loop;

  -- start tarvelers tasks
  for I in Process_Tasks'Range loop
    Process_Tasks(I).Start;
  end loop;

end Mutex_Template;
