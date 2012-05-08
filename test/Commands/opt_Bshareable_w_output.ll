; RUN: %MCLinker -mtriple="arm-none-linux-gnueabi" -march=arm \
; RUN: -filetype=obj -relocation-model=pic -dB %s -o %t.o
; RUN: %MCLinker -mtriple="arm-none-linux-gnueabi" -march=arm \
; RUN: -Bshareable %t.o -o %t.so
; RUN: test -f %t.so
; RUN: readelf -a %t.so | grep Type | grep DYN | grep "Shared object file"

define float @_Z1af(float %pInput) nounwind uwtable ssp {
entry:
  %pInput.addr = alloca float, align 4
  %a_fl = alloca float, align 4
  store float %pInput, float* %pInput.addr, align 4
  store float 0x3F947AE140000000, float* %a_fl, align 4
  %0 = load float* %a_fl, align 4
  %conv = fpext float %0 to double
  %mul = fmul double %conv, 3.000000e-01
  %conv1 = fptrunc double %mul to float
  store float %conv1, float* %a_fl, align 4
  %1 = load float* %a_fl, align 4
  %conv2 = fpext float %1 to double
  %add = fadd double %conv2, 1.300000e+00
  %conv3 = fptrunc double %add to float
  store float %conv3, float* %a_fl, align 4
  %2 = load float* %pInput.addr, align 4
  %3 = load float* %a_fl, align 4
  %mul4 = fmul float %3, %2
  store float %mul4, float* %a_fl, align 4
  %4 = load float* %a_fl, align 4
  ret float %4
}

define float @_Z2aaf(float %pInput) nounwind uwtable ssp {
entry:
  %pInput.addr = alloca float, align 4
  store float %pInput, float* %pInput.addr, align 4
  %0 = load float* %pInput.addr, align 4
  %conv = fpext float %0 to double
  %add = fadd double 1.100000e+00, %conv
  %conv1 = fptrunc double %add to float
  %call = call float @_Z1af(float %conv1)
  ret float %call
}
