program Project1;

{$mode objfpc}
{$modeswitch advancedrecords}

uses
  Classes, SysUtils;

procedure test;
var
    c: integer;
begin
    c := 10;
    writeln('test passed ', c);
end;

procedure main;
var
    b: single;
    items: array of single;
    i: integer;
    a: integer;
    items__it0: single;
    items__it0_index: integer;
    items__it1: single;
    items__it1_index: integer;
begin
    a := 5;
    b := a + 1.1;
    writeln('one', ' ', a + b);
    test();

    if a < 0 then begin
        writeln('a < 0');
    end
    else if a > 10 then begin
        writeln('a > 10');
    end
    else begin
        writeln('a >= 0 and <= 10');
        writeln(a + 10);
    end;
    i := 0;

    while true do begin
        writeln('i = ', i);
        i := i + 1;

        if i = 10 then begin
            break;
        end;
        //todo: if i = 10 do break
    end;
    SetLength(items, length(items) + 1);
    items[length(items) - 1] := 123;
    SetLength(items, length(items) + 1);
    items[length(items) - 1] := 456;
    SetLength(items, length(items) + 1);
    items[length(items) - 1] := 789;

    for items__it0_index := 0 to Length(items) - 1 do begin
        items__it0 := items[items__it0_index];
        writeln('>> ', items__it0_index, ' ', items__it0);

        for items__it1_index := 0 to Length(items) - 1 do begin
            items__it1 := items[items__it1_index];
            writeln('    ', items__it1_index, ' ', items__it1);
        end;
        writeln('<< ', items__it1_index, ' ', items__it1);
        writeln();
    end;
    // writeln(items[0]);
    // ret items
end;



begin
  main;
end.
