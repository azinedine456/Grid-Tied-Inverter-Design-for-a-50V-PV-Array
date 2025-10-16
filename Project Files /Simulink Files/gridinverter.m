clear all;
clc;
R= 1*10^-3;
L=2.4*10^-3;
Vpv =80;
Iref=11.96*sqrt(2);
phiref=0;
Vgrid=50*sqrt(2);
delay =3e-6;



sim("INVERTERCONTROL.slx");

try
    Ppv =mean(out.Ppv.Data)
    Pmosfet =mean(out.Pmosfet.Data)
    Pinv =mean(out.Pinv.Data)
    Pgrid =mean(out.Pgrid.Data)
catch
    Ppv =mean(ans.Ppv.Data)
    Pmosfet =mean(ans.Pmosfet.Data)
    Pinv =mean(ans.Pinv.Data)
    Pgrid =mean(ans.Pgrid.Data)
end