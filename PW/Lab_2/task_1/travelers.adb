with Ada.Text_IO; use Ada.Text_IO;
with Ada.Numerics.Float_Random; use Ada.Numerics.Float_Random;
with Random_Seeds; use Random_Seeds;
with Ada.Real_Time; use Ada.Real_Time;
with Ada.Wide_Characters.Handling;
with Ada.Characters.Handling; use Ada.Characters.Handling;
with Constants;

procedure Travelers is
  -- Global
  Start_Time : Time := Clock;
  Seeds : Seed_Array_Type(1 .. Constants.Nr_Of_Travelers + Constants.Nr_Of_Wild_Travelers) := Make_Seeds(Constants.Nr_Of_Travelers + Constants.Nr_Of_Wild_Travelers);

  -- Positions
  type Position_Type is record
    X : Integer range 0 .. Constants.Board_Width;
    Y : Integer range 0 .. Constants.Board_Height;
  end record;

  procedure Move_Down(Position : in out Position_Type) is
  begin
    Position.Y := (Position.Y + 1) mod Constants.Board_Height;
  end Move_Down;

  procedure Move_Up(Position : in out Position_Type) is
  begin
    Position.Y := (Position.Y + Constants.Board_Height - 1) mod Constants.Board_Height;
  end Move_Up;

  procedure Move_Right(Position : in out Position_Type) is
  begin
    Position.X := (Position.X + 1) mod Constants.Board_Width;
  end Move_Right;

  procedure Move_Left(Position : in out Position_Type) is
  begin
    Position.X := (Position.X + Constants.Board_Width - 1) mod Constants.Board_Width;
  end Move_Left;

  procedure Move_From(Position : in out Position_Type; Direction : Integer) is
  begin
    case Direction is
      when 0 => Move_Up(Position);
      when 1 => Move_Down(Position);
      when 2 => Move_Left(Position);
      when 3 => Move_Right(Position);
      when others => Put_Line(" UNKNOWN DIRECTION: " & Integer'Image(Direction));
    end case;
  end Move_From;

  -- Traces
  type Trace_Type is record
    Time_Stamp : Duration;
    Id         : Integer;
    Position   : Position_Type;
    Symbol     : Character;
  end record;

  type Trace_Array_Type is array (0 .. Constants.Max_Steps) of Trace_Type;

  type Traces_Sequence_Type is record
    Last        : Integer := -1;
    Trace_Array : Trace_Array_Type;
  end record;

  -- Printer
  procedure Print_Trace(Trace : Trace_Type) is
  begin
    Put_Line(Duration'Image(Trace.Time_Stamp) & " " &
             Integer'Image(Trace.Id) & " " &
             Integer'Image(Trace.Position.X) & " " &
             Integer'Image(Trace.Position.Y) & ( ' ', Trace.Symbol ));
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
    for I in 1 .. Constants.Nr_Of_Travelers + Constants.Nr_Of_Wild_Travelers loop
      accept Report(Traces : Traces_Sequence_Type) do
        Print_Traces(Traces);
      end Report;
    end loop;
  end Printer;

  -- Traveler Variants
  type Traveler_Variant is (Normal, Wild, None);

  type Traveler_Type is record
    Id       : Integer;
    Symbol   : Character;
    Position : Position_Type;
  end record;

  -- Task types for travelers
  task type Traveler_Task_Type is
    entry Init(Id : Integer; Seed : Integer; Symbol : Character);
    entry Start;
  end Traveler_Task_Type;

  task type Wild_Traveler_Task_Type is
    entry Init(Id : Integer; Seed : Integer; Symbol : Character);
    entry Start;
    entry Relocate(New_Position : Position_Type);
  end Wild_Traveler_Task_Type;

  -- General task type for travelers
  type General_Traveler_Task_Type (Variant : Traveler_Variant) is record
    case Variant is
      when Normal =>
        Traveler_Task : Traveler_Task_Type;
      when Wild =>
        Wild_Traveler_Task : Wild_Traveler_Task_Type;
      when None =>
        null;
    end case;
  end record;

  protected type Node is
    entry Init(New_Position : Position_Type);
    entry Enter(New_Traveler : access General_Traveler_Task_Type; Success : out Boolean);
    entry Move(New_Position : Position_Type; Success : out Boolean);
    entry Leave;
  private
    Initialized : Boolean := False;
    Traveler : access General_Traveler_Task_Type;
    Position : Position_Type;
  end Node;

  -- Global objects
  Board : array (0 .. Constants.Board_Width - 1, 0 .. Constants.Board_Height - 1) of Node;
  Travel_Tasks : array (0 .. Constants.Nr_Of_Travelers + Constants.Nr_Of_Wild_Travelers - 1) of access General_Traveler_Task_Type;
  Null_Task : access General_Traveler_Task_Type := new General_Traveler_Task_Type(Variant => None);
  
  -- Task bodies
  protected body Node is
    entry Init(New_Position : Position_Type) when not Initialized is
    begin
      Position := New_Position;
      Traveler := Null_Task;
      Initialized := True;
    end Init;

      -- can only enter if nothing there or wild traveler
    entry Enter(New_Traveler : access General_Traveler_Task_Type; Success : out Boolean) when Initialized and Traveler.Variant /= Normal is
      begin
      -- if nothing there, enter new traveler
      if Traveler.Variant = None then
        Traveler := New_Traveler;
        Success := True;

      -- if wild here, try to move him elsewhere
      elsif Traveler.Variant = Wild and New_Traveler.Variant = Normal then
        declare
          New_Position : Position_Type;
        begin
          for N in 0 .. 3 loop -- 4 directions
            -- try to move
            New_Position := Position;
            Move_From(New_Position, N);
            select
              -- try to enter
              Board(New_Position.X, New_Position.Y).Enter(Traveler, Success);
              if Success then -- if not success keep trying
                exit;
              end if;
            else -- blocks if Normal there - cant move
              Success := False;
            end select;
          end loop;

          if Success then -- notify wild to move
            Traveler.Wild_Traveler_Task.Relocate(New_Position);
            Traveler := New_Traveler;
          end if;
      end;

      -- if there is normal Traveler - refuse
      else
        Success := False;
      end if;
    end Enter;

    -- can only move a Normal traveler TODO
    entry Move(New_Position : Position_Type; Success : out Boolean) when Initialized and Traveler.Variant = Normal is
    begin
      -- locks if enter is locked so traveler can handle deadlock
      Board(New_Position.X, New_Position.Y).Enter(Traveler, Success);

      if Success then -- clear Node's Traveler
        Traveler := Null_Task;
      end if;
    end Move;

    entry Leave when Initialized is
    begin
      Traveler := Null_Task;
    end Leave;
  end Node;

  task body Traveler_Task_Type is
    G             : Generator;
    Traveler      : Traveler_Type;
    Time_Stamp    : Duration;
    Nr_of_Steps   : Integer;
    Traces        : Traces_Sequence_Type;

    New_Position  : Position_Type;
    Success       : Boolean;
    Deadlock      : Boolean;

    procedure Store_Trace is
    begin
      Traces.Last := Traces.Last + 1;
      Traces.Trace_Array(Traces.Last) := (
        Time_Stamp => Time_Stamp,
        Id         => Traveler.Id,
        Position   => Traveler.Position,
        Symbol     => Traveler.Symbol
      );
    end Store_Trace;

    procedure Make_Step(Position : in out Position_Type) is
      N : Integer;
    begin
      N := Integer(Float'Floor(4.0 * Random(G)));
      Move_From(Position, N);
    end Make_Step;

  begin
    accept Init(Id : Integer; Seed : Integer; Symbol : Character) do
      Reset(G, Seed);
      Nr_of_Steps := Constants.Min_Steps + Integer(Float(Constants.Max_Steps - Constants.Min_Steps) * Random(G));
      Traveler.Id := Id;
      Traveler.Symbol := Symbol;

      -- try start movement on the board (until it's successful)
      Success := False;
      while not Success loop
        Traveler.Position := ( -- random position
          X => Integer(Float'Floor(Float(Constants.Board_Width) * Random(G))),
          Y => Integer(Float'Floor(Float(Constants.Board_Height) * Random(G)))
        );
        select
          Board(Traveler.Position.X, Traveler.Position.Y).Enter(Travel_Tasks(Traveler.Id), Success);
        else
          null;
        end select;
      end loop;
      Time_Stamp := To_Duration(Clock - Start_Time);
      Store_Trace;
    end Init;
   
    -- start the task
    accept Start do
      null;
    end Start;

    Deadlock := False;
    for Step in 0 .. Nr_of_Steps loop
      delay Constants.Min_Delay + (Constants.Max_Delay - Constants.Min_Delay) * Duration(Random(G));

      -- try to move
      Success := False;
      Deadlock := False;
      while not Success loop
         -- prepare new position
        New_Position := Traveler.Position;
        Make_Step(New_Position);
         -- try to move, signal Node to do the logic
        select
          Board(New_Position.X, New_Position.Y).Enter(Travel_Tasks(Traveler.Id), Success);
        or
          -- Deadlock
          delay 5 * Constants.Max_Delay;
          Deadlock := True;
          exit;
        end select;
      end loop;

      if Deadlock then
        Traveler.Symbol := To_Lower(Traveler.Symbol);
        Time_Stamp := To_Duration(Clock - Start_Time);
        Store_Trace;
        exit;
      else
        Board(Traveler.Position.X, Traveler.Position.Y).Leave;
        Traveler.Position := New_Position;
        Time_Stamp := To_Duration(Clock - Start_Time);
        Store_Trace;
      end if;
    end loop;

    -- Send the traces to the printer
    Printer.Report(Traces);

  end Traveler_Task_Type;

   task body Wild_Traveler_Task_Type is
    G              : Generator;
    Traveler       : Traveler_Type;
    Time_Stamp     : Duration;
    Traces         : Traces_Sequence_Type;
    Time_Appear    : Duration;
    Time_Disappear : Duration;

    Success        : Boolean;

    procedure Store_Trace is
    begin
      Traces.Last := Traces.Last + 1;
      Traces.Trace_Array(Traces.Last) := (
        Time_Stamp => Time_Stamp,
        Id         => Traveler.Id,
        Position   => Traveler.Position,
        Symbol     => Traveler.Symbol
      );
    end Store_Trace;

   begin
      accept Init(Id : Integer; Seed : Integer; Symbol : Character) do
         Reset(G, Seed);
         Traveler.Id := Id;
         Traveler.Symbol := Symbol;
         Time_Appear := (Constants.Max_Delay * Constants.Max_Steps) * Duration(Random(G));
         Time_Disappear := Time_Appear + (Constants.Max_Delay * Constants.Max_Steps - Time_Appear) * Duration(Random(G));
      end Init;

      accept Start do
         null;
      end Start;

      delay Time_Appear;

      Success := False;
      while not Success loop
         Traveler.Position := (
         X => Integer(Float'Floor(Float(Constants.Board_Width) * Random(G))),
         Y => Integer(Float'Floor(Float(Constants.Board_Height) * Random(G)))
         );
         select
            Board(Traveler.Position.X, Traveler.Position.Y).Enter(Travel_Tasks(Traveler.Id), Success);
         else
            null;
         end select;
      end loop;
      Time_Stamp := To_Duration(Clock - Start_Time);
      Store_Trace;

      loop
         select
         -- Force Relocation, no checking if correct
         accept Relocate(New_Position : Position_Type) do
            Traveler.Position := New_Position;
            Time_Stamp := To_Duration(Clock - Start_Time);
            Store_Trace;
         end Relocate;
         or
         delay 0.1; -- keep checking time, if exceeded, leave
         if To_Duration(Clock - Start_Time) >= Time_Disappear then
            Board(Traveler.Position.X, Traveler.Position.Y).Leave;
            Traveler.Position := (X => Constants.Board_Width, Y => Constants.Board_Height);
            Time_Stamp := To_Duration(Clock - Start_Time);
            Store_Trace;
            exit;
         end if;
         end select;
      end loop;

      -- Send the traces to the printer
      Printer.Report(Traces);

   end Wild_Traveler_Task_Type;

  -- Local
  Symbol : Character;
  Id : Integer;

begin
  Put_Line(
    "-1 " &
    Integer'Image(Constants.Nr_Of_Travelers + Constants.Nr_Of_Wild_Travelers) & " " &
    Integer'Image(Constants.Board_Width) & " " &
    Integer'Image(Constants.Board_Height)
  );

  for I in 0 .. Constants.Board_Width - 1 loop
    for J in 0 .. Constants.Board_Height - 1 loop
      Board(I, J).Init((X => I, Y => J));
    end loop;
  end loop;

  Id := 0;
  Symbol := 'A';
  for I in 0 .. Constants.Nr_Of_Travelers - 1 loop
    Travel_Tasks(Id) := new General_Traveler_Task_Type(Variant => Normal);
    Travel_Tasks(Id).Traveler_Task.Init(Id, Seeds(Id + 1), Symbol);
    Symbol := Character'Succ(Symbol);
    Id := Id + 1;
  end loop;

  Symbol := '0';
  for I in 0 .. Constants.Nr_Of_Wild_Travelers - 1 loop
    Travel_Tasks(Id) := new General_Traveler_Task_Type(Variant => Wild);
    Travel_Tasks(Id).Wild_Traveler_Task.Init(Id, Seeds(Id + 1), Symbol);
    Symbol := Character'Succ(Symbol);
    Id := Id + 1;
  end loop;

  Id := 0;
  for I in 0 .. Constants.Nr_Of_Travelers - 1 loop
    Travel_Tasks(Id).Traveler_Task.Start;
    Id := Id + 1;
  end loop;

  for I in 0 .. Constants.Nr_Of_Wild_Travelers - 1 loop
    Travel_Tasks(Id).Wild_Traveler_Task.Start;
    Id := Id + 1;
  end loop;

end Travelers;