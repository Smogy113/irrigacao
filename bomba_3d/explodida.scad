// Vista explodida para o guia de montagem (só ilustração; comprimentos de eixo/tubo encurtados)
include <bomba.scad>
peca = "nenhuma";

g = 14;                         // espaço entre as peças
tubo_ilu = 80; eixo_ilu = 45;   // comprimentos encurtados só para a figura
lx = 48;                        // coluna das etiquetas

module etiqueta(t, z, x = lx) color("black") translate([x, -40, z]) rotate([90, 0, 0])
    linear_extrude(0.5) text(t, size = 6, font = "DejaVu Sans:style=Bold", valign = "center");
module seta(z, x0 = 22, x1 = lx - 3) color("black") translate([x0, -40, z - 0.3]) cube([x1 - x0, 0.5, 0.6]);

// alturas (de cima para baixo)
z_voluta = 0;
z_rotor  = z_voluta + voluta_h + 12;
z_tampa  = z_rotor + rotor_placa + rotor_pa_h + 12;
z_tubo   = z_tampa + tampa_total + g;
z_sup    = z_tubo + tubo_ilu + g + enc_h;          // origem do suporte (topo da borda)
z_acop_e = z_sup + z_face + luva_motor_h + g;
z_motor  = z_acop_e + acop_h + g + motor_ressalto_h + motor_eixo_comp;

// 1 motor
translate([0, 0, z_motor]) { color("silver") { cylinder(d = motor_d, h = motor_comp);
    translate([0,0,-motor_ressalto_h]) cylinder(d = motor_ressalto_d, h = motor_ressalto_h);
    translate([0,0,-motor_ressalto_h-motor_eixo_comp]) cylinder(d = motor_eixo_d, h = motor_eixo_comp); } }
etiqueta("1  MOTOR (eixo para baixo)", z_motor + motor_comp/2); seta(z_motor + motor_comp/2, motor_d/2 + 2);

// 2 acoplamento
color("gold") translate([0, 0, z_acop_e]) acoplamento();
etiqueta("2  ACOPLAMENTO", z_acop_e + acop_h/2 + 3);
color("black") translate([lx, -40, z_acop_e + acop_h/2 - 5]) rotate([90,0,0]) linear_extrude(0.5)
    text("porca M4 na fenda + parafuso M4", size = 4, font = "DejaVu Sans", valign = "center");
seta(z_acop_e + acop_h/2, acop_d/2 + 2);

// 3 suporte
color("orange") translate([-cx, 0, z_sup]) suporte();
etiqueta("3  SUPORTE + GARRA", z_sup + z_face);
color("black") translate([lx, -40, z_sup + z_face - 8]) rotate([90,0,0]) linear_extrude(0.5)
    text("2x M4 na braçadeira, 1x M4 na garra", size = 4, font = "DejaVu Sans", valign = "center");
seta(z_sup + z_face, luva_r + 10);

// 4 tubo
color("lightgray") translate([0, 0, z_tubo]) tubo(tubo_ilu);
etiqueta("4  TUBO-GUIA", z_tubo + tubo_ilu/2 + 3);
color("black") translate([lx, -40, z_tubo + tubo_ilu/2 - 5]) rotate([90,0,0]) linear_extrude(0.5)
    text("encaixa no suporte e na tampa", size = 4, font = "DejaVu Sans", valign = "center");
seta(z_tubo + tubo_ilu/2, tubo_de/2 + 2);

// 5 eixo (ao lado, vai por dentro do tubo)
ex = -95;
color("dimgray") translate([ex, 0, z_tubo + tubo_ilu + 20 - eixo_ilu]) hex(eixo_ef, eixo_ilu);
color("steelblue") translate([ex, 0, z_tubo + tubo_ilu + 20 - eixo_ilu - 10]) difference() { cylinder(d = 12.6, h = 20); translate([0,0,-1]) hex(eixo_ef + folga, 22); }
color("dimgray") translate([ex, 0, z_tubo + tubo_ilu + 20 - 2*eixo_ilu]) hex(eixo_ef, eixo_ilu);
color("black") translate([ex - 7, -40, z_tubo + tubo_ilu + 44]) rotate([90,0,0]) linear_extrude(0.5)
    text("5  EIXO + LUVA", size = 6, font = "DejaVu Sans:style=Bold", halign = "center");
color("black") translate([ex - 7, -40, z_tubo + tubo_ilu + 36]) rotate([90,0,0]) linear_extrude(0.5)
    text("(vai por dentro do tubo)", size = 4, font = "DejaVu Sans", halign = "center");

// 6 tampa
color("mediumseagreen") translate([0, 0, z_tampa]) tampa();
etiqueta("6  TAMPA (entrada de água)", z_tampa + tampa_total/2); seta(z_tampa + tampa_total/2, enc_de/2 + 2);

// 7 rotor
color("red") translate([0, 0, z_rotor]) rotor();
etiqueta("7  ROTOR (pás para cima)", z_rotor + 4); seta(z_rotor + 4, rotor_r + 2);

// 8 voluta
color("seagreen") translate([0, 0, z_voluta]) voluta();
etiqueta("8  VOLUTA (3x M4)", z_voluta + voluta_h/2); seta(z_voluta + voluta_h/2, 40);
