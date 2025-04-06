with Ada.Text_IO;              use Ada.Text_IO;
with Ada.Numerics.Float_Random; use Ada.Numerics.Float_Random;
with Random_Seeds;             use Random_Seeds;
with Ada.Real_Time;            use Ada.Real_Time;

procedure Travelers is

   -- Simulation parameters
   Nr_Of_Travelers : constant Integer := 15;
   Min_Steps       : constant Integer := 10;
   Max_Steps       : constant Integer := 100;
   Min_Delay       : constant Duration := 0.01;
   Max_Delay       : constant Duration := 0.05;
   Lock_Wait       : constant Duration := 0.1;  -- Timeout when trying to lock a tile

   Board_Width  : constant Integer := 15;
   Board_Height : constant Integer := 15;

   Start_Time : Time := Clock;

   Seeds : Seed_Array_Type(1..Nr_Of_Travelers) := Make_Seeds(Nr_Of_Travelers);

   ------------------------------------------------------------------------------
   -- Position on the board
   ------------------------------------------------------------------------------
   type Position_Type is record	
      X : Integer range 0 .. Board_Width - 1; 
      Y : Integer range 0 .. Board_Height - 1; 
   end record;

   ------------------------------------------------------------------------------
   -- Protected type representing a board tile.
   -- A tile can be "acquired" (if not already occupied) and then later "released."
   ------------------------------------------------------------------------------
   protected type Tile_Control is
      entry Acquire;
      procedure Release;
   private
      Occupied : Boolean := False;
   end Tile_Control;

   protected body Tile_Control is
      entry Acquire when not Occupied is
      begin
         Occupied := True;
      end Acquire;

      procedure Release is
      begin
         Occupied := False;
      end Release;
   end Tile_Control;

   ------------------------------------------------------------------------------
   -- The global board: an array of tiles.
   ------------------------------------------------------------------------------
   Tiles : array (0 .. Board_Width - 1, 0 .. Board_Height - 1) of Tile_Control;

   ------------------------------------------------------------------------------
   -- Basic movement procedures (with torus wrap-around)
   ------------------------------------------------------------------------------
   procedure Move_Down( Position : in out Position_Type ) is
   begin
      Position.Y := ( Position.Y + 1 ) mod Board_Height;
   end Move_Down;

   procedure Move_Up( Position : in out Position_Type ) is
   begin
      Position.Y := ( Position.Y + Board_Height - 1 ) mod Board_Height;
   end Move_Up;

   procedure Move_Right( Position : in out Position_Type ) is
   begin
      Position.X := ( Position.X + 1 ) mod Board_Width;
   end Move_Right;

   procedure Move_Left( Position : in out Position_Type ) is
   begin
      Position.X := ( Position.X + Board_Width - 1 ) mod Board_Width;
   end Move_Left;

   ------------------------------------------------------------------------------
   -- Tracing: record a traveler's journey.
   ------------------------------------------------------------------------------
   type Trace_Type is record 	      
      Time_Stamp : Duration;
      Id         : Integer;
      Position   : Position_Type;
      Symbol     : Character;
   end record;	   

   type Trace_Array_Type is array (0 .. Max_Steps) of Trace_Type;

   type Traces_Sequence_Type is record
      Last        : Integer := -1;
      Trace_Array : Trace_Array_Type;
   end record; 

   procedure Print_Trace( Trace : Trace_Type ) is
      Symbol_Str : String := ( ' ', Trace.Symbol );
   begin
      Put_Line(
         Duration'Image( Trace.Time_Stamp ) & " " &
         Integer'Image( Trace.Id ) & " " &
         Integer'Image( Trace.Position.X ) & " " &
         Integer'Image( Trace.Position.Y ) & " " &
         ( ' ', Trace.Symbol )
      );
   end Print_Trace;

   procedure Print_Traces( Traces : Traces_Sequence_Type ) is
   begin
      for I in 0 .. Traces.Last loop
         Print_Trace( Traces.Trace_Array( I ) );
      end loop;
   end Print_Traces;

   ------------------------------------------------------------------------------
   -- Printer task: collects and prints travelers' traces.
   ------------------------------------------------------------------------------
   task Printer is
      entry Report( Traces : Traces_Sequence_Type );
   end Printer;
  
   task body Printer is 
   begin
      for I in 1 .. Nr_Of_Travelers loop
         accept Report( Traces : Traces_Sequence_Type ) do
            Print_Traces( Traces );
         end Report;
      end loop;
   end Printer;

   ------------------------------------------------------------------------------
   -- Traveler type definition.
   ------------------------------------------------------------------------------
   type Traveler_Type is record
      Id       : Integer;
      Symbol   : Character;
      Position : Position_Type;
   end record;

   ------------------------------------------------------------------------------
   -- Traveler task type: each traveler is a separate task.
   ------------------------------------------------------------------------------
   task type Traveler_Task_Type is	
      entry Init( Id   : Integer;
                  Seed : Integer;
                  Symbol : Character );
      entry Start;
   end Traveler_Task_Type;	

   task body Traveler_Task_Type is
      G : Generator;
      Traveler : Traveler_Type;
      Time_Stamp : Duration;
      Nr_of_Steps : Integer;
      Traces : Traces_Sequence_Type;

      -- Exception to signal a timeout (could not acquire a tile in time)
      Timeout_Occurred : exception;

      ----------------------------------------------------------------------------
      -- Store_Trace: Record the current state (time, position, symbol) in the trace.
      ----------------------------------------------------------------------------
      procedure Store_Trace is
      begin  
         Traces.Last := Traces.Last + 1;
         Traces.Trace_Array( Traces.Last ) :=
           ( Time_Stamp => Time_Stamp,
             Id         => Traveler.Id,
             Position   => Traveler.Position,
             Symbol     => Traveler.Symbol );
      end Store_Trace;

      ----------------------------------------------------------------------------
      -- Attempt_Move: Compute a candidate move and try to acquire the candidate tile
      -- using a timed entry call. On success, release the current tile, update the
      -- traveler's position, and record the move. On timeout, change the symbol to
      -- lowercase, record the state, and raise an exception.
      ----------------------------------------------------------------------------
      procedure Attempt_Move is
         N : Integer;
         Candidate : Position_Type := Traveler.Position;
      begin
         N := Integer( Float'Floor( 4.0 * Random(G) ) );
         case N is
            when 0 =>
               -- Move Up
               Candidate.Y := ( Traveler.Position.Y + Board_Height - 1 ) mod Board_Height;
            when 1 =>
               -- Move Down
               Candidate.Y := ( Traveler.Position.Y + 1 ) mod Board_Height;
            when 2 =>
               -- Move Left
               Candidate.X := ( Traveler.Position.X + Board_Width - 1 ) mod Board_Width;
            when 3 =>
               -- Move Right
               Candidate.X := ( Traveler.Position.X + 1 ) mod Board_Width;
            when others =>
               Put_Line( "?????????????? " & Integer'Image( N ) );
         end case;

         if Tiles(Candidate.X, Candidate.Y).Acquire'Timed_Wait( Lock_Wait ) then
            -- Successfully acquired candidate tile:
            Tiles(Traveler.Position.X, Traveler.Position.Y).Release;
            Traveler.Position := Candidate;
            Time_Stamp := To_Duration( Clock - Start_Time );
            Store_Trace;
         else
            -- Timeout: change symbol to lowercase and record final state.
            Traveler.Symbol := Character'Val( Character'Pos(Traveler.Symbol) + 32 );
            Time_Stamp := To_Duration( Clock - Start_Time );
            Store_Trace;
            raise Timeout_Occurred;
         end if;
      end Attempt_Move;

   begin
      accept Init( Id : Integer; Seed : Integer; Symbol : Character ) do
         Reset( G, Seed );
         Traveler.Id     := Id;
         Traveler.Symbol := Symbol;
         -- Determine a random starting position.
         Traveler.Position :=
           ( X => Integer( Float'Floor( Float( Board_Width ) * Random(G) ) ),
             Y => Integer( Float'Floor( Float( Board_Height ) * Random(G) ) ) );
         -- Acquire the starting tile.
         Tiles( Traveler.Position.X, Traveler.Position.Y ).Acquire;
         Time_Stamp := To_Duration( Clock - Start_Time );
         Store_Trace;
         Nr_of_Steps := Min_Steps + Integer( Float( Max_Steps - Min_Steps ) * Random(G) );
      end Init;
      
      accept Start do
         null;
      end Start;

      -- Main movement loop.
      for Step in 0 .. Nr_of_Steps loop
         delay Min_Delay + (Max_Delay - Min_Delay)*Duration( Random(G) );
         begin
            Attempt_Move;
         exception
            when Timeout_Occurred =>
               exit;  -- Exit loop on timeout.
         end;
      end loop;
      -- Report the traveler's trace (whether complete or early terminated)
      Printer.Report( Traces );
   end Traveler_Task_Type;

   ------------------------------------------------------------------------------
   -- Main task: create, initialize, and start traveler tasks.
   ------------------------------------------------------------------------------
   Travel_Tasks : array (0 .. Nr_Of_Travelers - 1) of Traveler_Task_Type;
   Symbol       : Character := 'A';
begin 
   -- Print simulation parameters.
   Put_Line(
      "-1 " &
      Integer'Image( Nr_Of_Travelers ) & " " &
      Integer'Image( Board_Width ) & " " &
      Integer'Image( Board_Height )
   );

   -- Initialize traveler tasks with unique seeds and symbols.
   for I in Travel_Tasks'Range loop
      Travel_Tasks(I).Init( I, Seeds(I+1), Symbol );
      Symbol := Character'Succ( Symbol );
   end loop;

   -- Start all traveler tasks.
   for I in Travel_Tasks'Range loop
      Travel_Tasks(I).Start;
   end loop;
end Travelers;

