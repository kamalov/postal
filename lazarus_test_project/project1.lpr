program project1;

{$mode objfpc}{$H+}
{$modeswitch advancedrecords}

uses
  Classes, SysUtils;

type
  generic TRefCount<TSomeType> = record
    private type PSomeType = ^TSomeType;
    private
      ptr: PSomeType;
      class operator Initialize(var aSelf: TRefCount);
      class operator Finalize(var aSelf: TRefCount);
      class operator AddRef(var aSelf: TRefCount);
      class operator Copy(constref aSrc: TRefCount; var aDst: TRefCount);
  end;

  PTestRec = ^TTestRec;

  TTestRec = record
  public
    rc: integer;
    i: integer;
  end;

  TTestRec1 = record
  public
    rc: integer;
    i: integer;
  end;

  TTestRecRc = specialize TRefCount<TTestRec>;
  TTestRecRc1 = specialize TRefCount<TTestRec1>;

  function ptoi(p: Pointer): PtrInt;
  begin
    Result := PtrInt(p);
  end;

  class operator TRefCount.Initialize(var aSelf: TRefCount);
  begin
    aSelf.ptr := New(PSomeType);
    aSelf.ptr^.rc := 1;
    WriteLn('.Initialize ', ptoi(aSelf.ptr));
  end;

  class operator TRefCount.Finalize(var aSelf: TRefCount);
  begin
    if aSelf.ptr = nil then begin
        WriteLn('.Finalize ', ptoi(aSelf.ptr));
    end
    else begin
        WriteLn('.Finalize ', ptoi(aSelf.ptr), ' ', aSelf.ptr^.rc);
    end;

    if aSelf.ptr = nil then exit;

    if aSelf.ptr^.rc = 0 then begin
       raise Exception.Create('panic!');
    end;

    aSelf.ptr^.rc := aSelf.ptr^.rc - 1;
    if aSelf.ptr^.rc = 0 then begin
       WriteLn('Dispose ', ptoi(aSelf.ptr));
       Dispose(aSelf.ptr);
    end;
  end;

  class operator TRefCount.AddRef(var aSelf: TRefCount);
  begin
    WriteLn('.AddRef ', ptoi(aSelf.ptr));
    raise Exception.Create('panic!');
  end;

  class operator TRefCount.Copy(constref aSrc: TRefCount; var aDst: TRefCount);
  begin
    WriteLn('.Copy ', ptoi(aSrc.ptr), ' -> ', ptoi(aDst.ptr));
    if aDst.ptr <> nil then begin
      aDst.ptr^.rc := aDst.ptr^.rc - 1;
    end;

    aDst.ptr := aSrc.ptr;

    if aDst.ptr <> nil then begin
      aDst.ptr^.rc := aDst.ptr^.rc + 1;
    end;
  end;

function SubTest: TTestRecRC;
var
  a: TTestRecRC;
  b: TTestRecRC;
begin
  a.ptr^.i := 10;
  b := a;
  result := b;
end;

procedure Test;
var
  c: TTestRecRC;
begin
  c := SubTest;
end;

type
  generic TDynArray<TType> = record
    _arr: array of TType;
    procedure push(v: TType);
    function pop(): TType;
    class operator Initialize(var aSelf: TDynArray);
    class operator Finalize(var aSelf: TDynArray);
  end;

  class operator TDynArray.Initialize(var aSelf: TDynArray);
  begin
    SetLength(aSelf._arr, 0);
  end;

  class operator TDynArray.Finalize(var aSelf: TDynArray);
  begin
    SetLength(aSelf._arr, 0);
  end;

  procedure TDynArray.push(v: TType);
  var
    new_length: integer;
  begin
    new_length := length(_arr) + 1;
    SetLength(_arr, new_length);
    _arr[new_length - 1] := v;
  end;

  function TDynArray.pop(): TType;
  begin
    result := _arr[length(_arr) - 1];
    SetLength(_arr, length(_arr) - 1);
  end;

type
  generic TArray<T> = array of T;

  generic procedure push<T>(var arr: specialize TArray<T>; value: T);
  begin
    SetLength(arr, length(arr) + 1);
    arr[length(arr) - 1] := value;
  end;


  generic function pop<T>(var arr: specialize TArray<T>): T;
  begin
    result := arr[length(arr) - 1];
    SetLength(arr, length(arr) - 1);
  end;

type
    TIntDynArray = specialize TDynArray<integer>;

var
   a: specialize TDynArray<integer>;
   b: specialize TArray<integer>;
   c: integer;
   d: array of integer;
begin
  //setlength(b, 0);
  //specialize push<integer>(b, 123);
  //specialize push<integer>(b, 123);
  //writeln(b[1]);
  //writeln(length(b));
  //c := specialize pop<integer>(b);
  //writeln(c);
  //specialize pop<integer>(b);
  //writeln(length(b));

  setlength(d, 1);
  d[0] := 123;
  writeln(length(d));
  writeln(d[0]);
  ReadLn();
end.

//program project1;
//
//{$mode objfpc}
//
//uses
//  Classes, SysUtils;
//
//Type
//  TIntArray = Array of Integer;
//
//procedure main;
//var
//    a: single;
//    b: integer;
//    _a_index: integer;
//    col: TIntArray;
//begin
//    a := 1 * 2 + 3 * 4 / 5 + 6;
//    // var a: float = 1*2 + 3*4/5 + 6;
//    b := 1 * 2 + 3 * 4;
//    writeln('one', ' ', a + b);
//
//    if b > 0 then begin
//        writeln('two');
//        writeln('three');
//    end;
//
//    SetLength(col, 2);
//    for _a_index := Low(col) to High(col) do begin
//        a := (col)[_a_index];
//        writeln(a);
//    end;
//end;


begin
  main;
end.
