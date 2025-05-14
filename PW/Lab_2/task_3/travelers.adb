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
  Seeds : Seed_Array_Type(1 .. Constants.Nr_Of_Travelers + Constants.Nr_Of_Wild_Travelers + Constants.Nr_Of_Traps)
    := Make_Seeds(Constants.Nr_Of_Travelers + Constants.Nr_Of_Wild_Travelers + Constants.Nr_Of_Traps);
  Finish : Boolean := False;

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
    -- To end the program
    Finish := True;

    for I in 1 .. Constants.Nr_Of_Traps loop
      accept Report(Traces : Traces_Sequence_Type) do
        Print_Traces(Traces);
      end Report;
    end loop;

  end Printer;

  -- Traveler Variants
  type Traveler_Variant is (Normal, Wild, Trap, None);

  type Traveler_Type is record
    Id       : Integer;
    Symbol   : Character;
    Position : Position_Type;
  end record;

  -- Handy Response types
  type Response_Type is (Success, Fail, Trap, Deadlock);

  -- Task types for travelers
  task type Traveler_Task_Type is
    entry Init(Id : Integer; Seed : Integer; Symbol : Character);
    entry Start;
  end Traveler_Task_Type;

  task type Wild_Traveler_Task_Type is
    entry Init(Id : Integer; Seed : Integer; Symbol : Character);
    entry Start;
    entry Relocate(New_Position : Position_Type; New_Response : Response_Type);
  end Wild_Traveler_Task_Type;

  task type Trap_Task_Type is
    entry Init(Id : Integer; Seed : Integer);
    entry Trapped(Id : Integer; New_Response : in out Response_Type);
  end Trap_Task_Type;

  -- General task type for travelers
  type General_Traveler_Task_Type (Variant : Traveler_Variant) is record
    Traveler : aliased Traveler_Type;
    case Variant is
      when Normal =>
        Traveler_Task : Traveler_Task_Type;
      when Wild =>
        Wild_Traveler_Task : Wild_Traveler_Task_Type;
      when Trap =>
        Trap_Task : Trap_Task_Type;
      when None =>
        null;
     end case;
  end record;

   protected type Node is
    entry Init(New_Position : Position_Type);
    entry Enter(Id : Integer; Response : in out Response_Type);
    entry Leave;
  private
    Initialized : Boolean := False;
    Traveler : access General_Traveler_Task_Type;
    Position : Position_Type;
  end Node;

  -- Global objects
  Board : array (0 .. Constants.Board_Width - 1, 0 .. Constants.Board_Height - 1) of Node;
  Travel_Tasks : array (0 .. Constants.Nr_Of_Travelers + Constants.Nr_Of_Wild_Travelers + Constants.Nr_Of_Traps - 1) of access General_Traveler_Task_Type;
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
     entry Enter(Id : Integer; Response : in out Response_Type) when Initialized and Traveler.Variant /= Normal is New_Traveler : access General_Traveler_Task_Type;
      begin
        New_Traveler := Travel_Tasks(Id);

      -- if nothing there, enter new traveler
      if Traveler.Variant = None then
        Traveler := New_Traveler;
        Response := Success;

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
               Board(New_Position.X, New_Position.Y).Enter(Traveler.Traveler.Id, Response);
            else -- blocks if Normal there - cant move
              Response := Fail;
            end select;
               exit when Response /= Fail;
          end loop;

          if Response /= Fail then -- signal wild and assign new traveler here
            if Response /= Trap then -- if trapped task might not exist
              Traveler.Wild_Traveler_Task.Relocate(New_Position, Success);
            end if;
            Traveler := New_Traveler;
            Response := Success;
          end if;
      end;

      -- if trap move logic to trap task
      elsif Traveler.Variant = Trap then
        Traveler.Trap_Task.Trapped(Id, Response);

      -- if normal traveler - refuse
      else
        Response := Fail;
      end if;
    end Enter;

    entry Leave when Initialized is
    begin
      Traveler := Null_Task;
    end Leave;
  end Node;

  task body Traveler_Task_Type is
    G             : Generator;
    Traveler      : access Traveler_Type;
    Time_Stamp    : Duration;
    Nr_of_Steps   : Integer;
    Traces        : Traces_Sequence_Type;

    New_Position  : Position_Type;
    Response      : Response_Type;

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
      Traveler := Travel_Tasks(Id).Traveler'Access;
      Traveler.Id := Id;
      Traveler.Symbol := Symbol;

      -- try to move in
      Response := Fail;
      while Response = Fail loop
        Traveler.Position := ( -- random position
          X => Integer(Float'Floor(Float(Constants.Board_Width) * Random(G))),
          Y => Integer(Float'Floor(Float(Constants.Board_Height) * Random(G)))
        );
        select
          Board(Traveler.Position.X, Traveler.Position.Y).Enter(Traveler.Id, Response);
        else
          null;
        end select;
      end loop;

      if Response = Trap then -- leave board
        Traveler.Position := (Constants.Board_Width, Constants.Board_Height);
      end if;

      Time_Stamp := To_Duration(Clock - Start_Time);
      Store_Trace;
    end Init;

    accept Start do
      null;
    end Start;

    for Step in 0 .. Nr_of_Steps loop
      exit when Response = Trap or Response = Deadlock;
      delay Constants.Min_Delay + (Constants.Max_Delay - Constants.Min_Delay) * Duration(Random(G));

      -- try to move
      Response := Fail;
      while Response = Fail loop
        New_Position := Traveler.Position;
        Make_Step(New_Position);
        select
          Board(New_Position.X, New_Position.Y).Enter(Traveler.Id, Response);
        or
          delay 6 * Constants.Max_Delay; -- deadlock
          Response := Deadlock;
        end select;
      end loop;

      -- handle response
      -- Maybe i should kill when Trapped?
      case Response is
        when Success =>
          Board(Traveler.Position.X, Traveler.Position.Y).Leave;
          Traveler.Position := New_Position;
        when Trap =>
          Board(Traveler.Position.X, Traveler.Position.Y).Leave;
          Traveler.Position := (Constants.Board_Width, Constants.Board_Height);
        when Deadlock =>
          Traveler.Symbol := To_Lower(Traveler.Symbol);
        when others =>
          null;
      end case;

      -- store trace
      Time_Stamp := To_Duration(Clock - Start_Time);
      Store_Trace;
    end loop;

    -- Send the traces to the printer
    Printer.Report(Traces);

  end Traveler_Task_Type;

   task body Wild_Traveler_Task_Type is
    G              : Generator;
    Traveler       : access Traveler_Type;
    Time_Stamp     : Duration;
    Traces         : Traces_Sequence_Type;
    Time_Appear    : Duration;
    Time_Disappear : Duration;

    Response       : Response_Type;

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
      Time_Appear := (Constants.Max_Delay * Constants.Max_Steps) * Duration(Random(G));
      Time_Disappear := Time_Appear + (Constants.Max_Delay * Constants.Max_Steps - Time_Appear) * Duration(Random(G));
      Traveler := Travel_Tasks(Id).Traveler'Access;
      Traveler.Id := Id;
      Traveler.Symbol := Symbol;
    end Init;

    accept Start do
      null;
    end Start;

    delay Time_Appear;
    
    Response := Fail;
    while Response = Fail loop
      Traveler.Position := ( -- random initial position
        X => Integer(Float'Floor(Float(Constants.Board_Width) * Random(G))),
        Y => Integer(Float'Floor(Float(Constants.Board_Height) * Random(G)))
      );
      select
        Board(Traveler.Position.X, Traveler.Position.Y).Enter(Traveler.Id, Response);
      else
        null;
      end select;
    end loop;

    Time_Stamp := To_Duration(Clock - Start_Time);
    Store_Trace;

    loop -- main loop
      exit when Response = Trap or To_Duration(Clock - Start_Time) >= Time_Disappear;

      select -- forceful relocation
        accept Relocate(New_Position : Position_Type; New_Response : Response_Type) do
          Response := New_Response;
          Traveler.Position := New_Position;
          Time_Stamp := To_Duration(Clock - Start_Time);
          Store_Trace;
        end Relocate;
      or
        delay 0.1; -- keep checking time
      end select;
    end loop;

    -- free the board
    if Response /= Trap then
      Board(Traveler.Position.X, Traveler.Position.Y).Leave;
      Traveler.Position := (Constants.Board_Width, Constants.Board_Height);
      Time_Stamp := To_Duration(Clock - Start_Time);
      Store_Trace;
    end if;

    -- Send the traces to the printer
    Printer.Report(Traces);
  end Wild_Traveler_Task_Type;

   task body Trap_Task_Type is
    G             : Generator;
    Trap_Traveler : access Traveler_Type;
    Time_Stamp    : Duration;
    Traces        : Traces_Sequence_Type;
    Response      : Response_Type;
    Traveler      : access General_Traveler_Task_Type := Null_Task;

    procedure Store_Trace is
    begin
      Traces.Last := Traces.Last + 1;
      Traces.Trace_Array(Traces.Last) := (
        Time_Stamp => Time_Stamp,
        Id         => Trap_Traveler.Id,
        Position   => Trap_Traveler.Position,
        Symbol     => Trap_Traveler.Symbol
      );
    end Store_Trace;

  begin
    accept Init(Id : Integer; Seed : Integer) do
      Reset(G, Seed);
      Trap_Traveler := Travel_Tasks(Id).Traveler'Access;
      Trap_Traveler.Id := Id;
      Trap_Traveler.Symbol := '#';

      -- try to move in
      Response := Fail;
      while Response = Fail loop
        Trap_Traveler.Position := ( -- random position
          X => Integer(Float'Floor(Float(Constants.Board_Width) * Random(G))),
          Y => Integer(Float'Floor(Float(Constants.Board_Height) * Random(G)))
        );
        select
          Board(Trap_Traveler.Position.X, Trap_Traveler.Position.Y).Enter(Trap_Traveler.Id, Response);
        else
          null;
        end select;
      end loop;

      Time_Stamp := To_Duration(Clock - Start_Time);
      Store_Trace;
    end Init;

    loop
      exit when Finish; -- stop catching the travelers

      select -- catch traveler
        accept Trapped(Id : Integer; New_Response : in out Response_Type) do
          Traveler := Travel_Tasks(Id);
          case Traveler.Variant is
            when Normal =>
              Response := Trap;
              Trap_Traveler.Symbol := To_Lower(Traveler.Traveler.Symbol);
              -- Traveler.Traveler.Store_Trace; or ssmtyh
              -- i should try sending the move first so it would store and then move
            when Wild =>
              select -- nudge Wild
                Traveler.Wild_Traveler_Task.Relocate((Constants.Board_Width, Constants.Board_Height), Trap);
                Response := Trap;
                Trap_Traveler.Symbol := '*';
              else
                Response := Fail;
              end select;
            when others =>
              Response := Fail;
          end case;
          New_Response := Response;
        end Trapped;

        -- if traveler trapped
        if Response = Trap then
          Time_Stamp := To_Duration(Clock - Start_Time);
          Store_Trace;
          -- block
          delay 4 * Constants.Max_Delay; -- example amount of time
          -- go back
          Trap_Traveler.Symbol := '#';
          Time_Stamp := To_Duration(Clock - Start_Time);
          Store_Trace;
        end if;
      or
        delay 0.1;
      end select;
    end loop;

    Printer.Report(Traces);

  end Trap_Task_Type;

  -- Local in Task
  Symbol : Character;
  Id : Integer;

begin
  Put_Line(
    "-1 " &
    Integer'Image(Constants.Nr_Of_Travelers + Constants.Nr_Of_Wild_Travelers + Constants.Nr_Of_Traps) & " " &
    Integer'Image(Constants.Board_Width) & " " &
    Integer'Image(Constants.Board_Height)
  );

  for I in 0 .. Constants.Board_Width - 1 loop
    for J in 0 .. Constants.Board_Height - 1 loop
      Board(I, J).Init((X => I, Y => J));
    end loop;
  end loop;

  Id := 0;
  for I in 0 .. Constants.Nr_Of_Traps - 1 loop
    Travel_Tasks(Id) := new General_Traveler_Task_Type(Variant => Trap);
    Travel_Tasks(Id).Trap_Task.Init(Id, Seeds(Id + 1));
    Id := Id + 1;
  end loop;

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

   Id := Constants.Nr_Of_Traps;
  for I in 0 .. Constants.Nr_Of_Travelers - 1 loop
    Travel_Tasks(Id).Traveler_Task.Start;
    Id := Id + 1;
  end loop;

  for I in 0 .. Constants.Nr_Of_Wild_Travelers - 1 loop
    Travel_Tasks(Id).Wild_Traveler_Task.Start;
    Id := Id + 1;
  end loop;

end Travelers;