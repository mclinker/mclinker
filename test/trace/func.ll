; ModuleID = './func.cpp'
target datalayout = "e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:64:128-a0:0:64-n32"
target triple = "armv4t-none-linux-gnueabi"

%0 = type { i8*, i8* }
%1 = type { i32, void ()* }
%class.A = type { i32 (...)** }
%class.B = type { i8 }

@g_GV = global i32 10, align 4
@_ZN1A1aE = global i32 10, align 4
@_ZL4g_SV = internal global i32 10, align 4
@_ZTV1A = unnamed_addr constant [3 x i8*] [i8* null, i8* bitcast (%0* @_ZTI1A to i8*), i8* bitcast (void (%class.A*)* @_ZN1A1fEv to i8*)]
@_ZTVN10__cxxabiv117__class_type_infoE = external global i8*
@_ZTS1A = constant [3 x i8] c"1A\00"
@_ZTI1A = unnamed_addr constant %0 { i8* bitcast (i8** getelementptr inbounds (i8** @_ZTVN10__cxxabiv117__class_type_infoE, i32 2) to i8*), i8* getelementptr inbounds ([3 x i8]* @_ZTS1A, i32 0, i32 0) }
@llvm.global_ctors = appending global [1 x %1] [%1 { i32 65535, void ()* bitcast (i32 (i32)* @_Z1gi to void ()*) }]
@llvm.global_dtors = appending global [1 x %1] [%1 { i32 65535, void ()* bitcast (void (i32)* @_Z1fi to void ()*) }]

@_ZN1BC1Ev = alias %class.B* (%class.B*)* @_ZN1BC2Ev

define void @_Z1fi(i32 %pArg1) nounwind {
entry:
  %pArg1.addr = alloca i32, align 4
  store i32 %pArg1, i32* %pArg1.addr, align 4
  %tmp = load i32* %pArg1.addr, align 4
  store i32 %tmp, i32* @g_GV, align 4
  ret void
}

define i32 @_Z1gi(i32 %pArg1) nounwind {
entry:
  %pArg1.addr = alloca i32, align 4
  store i32 %pArg1, i32* %pArg1.addr, align 4
  %tmp = load i32* @_ZL4g_SV, align 4
  %tmp1 = load i32* %pArg1.addr, align 4
  %add = add nsw i32 %tmp, %tmp1
  ret i32 %add
}

define void @_ZN1A1fEv(%class.A* %this) nounwind align 2 {
entry:
  %this.addr = alloca %class.A*, align 4
  store %class.A* %this, %class.A** %this.addr, align 4
  %this1 = load %class.A** %this.addr
  ret void
}

define %class.B* @_ZN1BC2Ev(%class.B* %this) unnamed_addr nounwind align 2 {
entry:
  %this.addr = alloca %class.B*, align 4
  store %class.B* %this, %class.B** %this.addr, align 4
  %this1 = load %class.B** %this.addr
  ret %class.B* %this1
}
