// =====================================================================
//  Bomba d'água de eixo longo para irrigação
//  - Motor fica SECO, preso na borda do recipiente por uma garra
//  - Eixo impresso (sextavado) leva o giro até o rotor submerso
//  - Bomba centrífuga com voluta e bico para mangueira
//
//  Como usar:
//   1. Ajuste os PARÂMETROS abaixo com as suas medidas
//   2. Escolha a peça em `peca` (ou use o Customizer do OpenSCAD)
//   3. F6 (Render) e depois F7 (Exportar STL)
//  Pela linha de comando:
//   openscad -D 'peca="rotor"' -o rotor.stl bomba.scad
// =====================================================================

/* [Peça a exibir/exportar] */
peca = "montagem"; // [montagem, suporte, acoplamento, eixo, luva_eixo, tubo, luva_tubo, rotor, voluta, tampa]

/* [Motor (medir com paquímetro!)] */
motor_d          = 20.2; // diâmetro do corpo
motor_chato      = 15.1; // distância entre os lados chatos (igual a motor_d se for redondo)
motor_comp       = 25;   // comprimento do corpo (sem eixo)
motor_ressalto_d = 6.2;  // diâmetro do ressalto/mancal na frente do motor
motor_eixo_d     = 2.0;  // diâmetro do eixo do motor
motor_eixo_comp  = 8;    // quanto o eixo sai para fora do corpo

/* [Recipiente] */
borda_max    = 12;  // espessura MÁXIMA de parede que a garra aceita (o parafuso aperta paredes mais finas)
profundidade = 200; // da parte de cima da borda até o fundo da bomba

/* [Mangueira] */
mangueira_di = 4;   // diâmetro INTERNO da mangueira

/* [Impressão] */
folga        = 0.3; // folga para encaixes
altura_max   = 200; // maior peça que sua impressora faz (eixo/tubo são divididos se passar disso)
$fn = 72;

// ===================== DERIVADOS (não precisa mexer) ==================
eixo_ef   = 7;                    // eixo sextavado: medida entre faces
eixo_circ = eixo_ef / cos(30);    // diâmetro entre cantos
bucha_d   = eixo_circ + 0.7;      // furo-mancal por onde o eixo gira

tubo_de = 20;  tubo_di = 14.4;    // tubo-guia (coluna)
enc_di  = tubo_de + folga + 0.1;  // encaixe do tubo
enc_de  = enc_di + 6;
enc_h   = 25;                     // encaixe no suporte

// --- bomba ---
rotor_r = 15; rotor_placa = 2; rotor_pa_h = 6; rotor_pas = 6; cubo_d = 12;
piso = 2; camara_h = rotor_placa + rotor_pa_h + 2; tampa_e = 3;
r0 = rotor_r + 1.5; voluta_larg = 6; Rmax = r0 + voluta_larg;
corpo_r = Rmax + 8; furo_r = Rmax + 3.5;
xc_saida = r0 + voluta_larg/2;    // centro do canal de saída
bico_larg = voluta_larg + 6;
voluta_h = piso + camara_h;
olho_d = 16;                      // entrada de água (em volta do eixo)
pernas_h = 14; enc_piso = 3; enc_prof = 15;
tampa_total = tampa_e + pernas_h + enc_piso + enc_prof;
z_piso_bomba = voluta_h + tampa_e + pernas_h + enc_piso; // topo do mancal inferior (local)

// --- suporte ---
placa_h = 6; perna_e = 8;
cx = max(enc_de/2, corpo_r + 3);  // centro do eixo: longe o bastante para a bomba não bater na parede
luva_r = max(motor_d, motor_chato)/2 + folga + 2.5;
acop_d = 13; acop_hex_prof = 10; acop_sep = 2;
z_acop = placa_h + 2;
z_face = z_acop + acop_hex_prof + acop_sep + motor_eixo_comp; // face do motor
luva_motor_h = motor_comp * 0.8;

// --- comprimentos ---
tubo_comp = profundidade - z_piso_bomba;
eixo_topo = z_acop + acop_hex_prof - 0.5;
eixo_base = -profundidade + piso + 1 + 1;           // fundo do encaixe do rotor
eixo_comp = eixo_topo - eixo_base;
n_eixo = ceil(eixo_comp / altura_max);  seg_eixo = eixo_comp / n_eixo;
n_tubo = ceil(tubo_comp / altura_max);  seg_tubo = tubo_comp / n_tubo;

echo(str("== Eixo total ", eixo_comp, " mm -> imprimir ", n_eixo, " x ", seg_eixo, " mm + ", n_eixo-1, " luva_eixo"));
echo(str("== Tubo total ", tubo_comp, " mm -> imprimir ", n_tubo, " x ", seg_tubo, " mm + ", n_tubo-1, " luva_tubo"));
assert(tubo_comp > 20, "profundidade pequena demais");

// ============================ UTIL ===================================
module hex(ef, h) cylinder(d = ef/cos(30), h = h, $fn = 6);
module perfil_motor(o = 0)
    intersection() { circle(d = motor_d + 2*o); square([motor_chato + 2*o, motor_d + 2*o + 1], center = true); }

// ============================ PEÇAS ==================================

// ---------- SUPORTE com garra + berço do motor ----------
// Origem: topo da borda do recipiente; parede ocupa x = [-espessura, 0]
module suporte() {
    xl = -borda_max - perna_e;                       // face externa da perna de fora
    difference() {
        union() {
            // placa que apoia na borda
            hull() {
                translate([xl, -10, 0]) cube([perna_e, 20, placa_h]);
                translate([cx, 0, 0]) cylinder(r = max(luva_r, enc_de/2), h = placa_h);
            }
            // perna de fora (com parafuso de aperto)
            translate([xl, -10, -enc_h]) cube([perna_e, 20, enc_h + placa_h]);
            // encaixe do tubo
            translate([cx, 0, -enc_h]) cylinder(d = enc_de, h = enc_h);
            // perna de dentro (encosta na parede) ligada ao encaixe
            translate([0, -10, -enc_h]) cube([cx, 20, enc_h]);
            // câmara do acoplamento
            translate([cx, 0, placa_h]) cylinder(r = luva_r, h = z_face - placa_h);
            // berço do motor
            translate([cx, 0, z_face]) linear_extrude(luva_motor_h) perfil_motor(folga + 2.5);
            // orelhas da braçadeira
            translate([cx - 5, luva_r - 3, z_face]) cube([10, 10, luva_motor_h]);
        }
        // encaixe do tubo
        translate([cx, 0, -enc_h - 1]) cylinder(d = enc_di, h = enc_h + 1);
        // mancal superior
        translate([cx, 0, -1]) cylinder(d = bucha_d, h = placa_h + 2);
        // interior da câmara
        translate([cx, 0, placa_h]) cylinder(r = luva_r - 2.5, h = z_face - placa_h - 3);
        // janelas para alcançar o parafuso do acoplamento
        translate([cx - 30, -6, placa_h + 1]) cube([60, 12, z_face - placa_h - 5]);
        // furo do ressalto do motor
        translate([cx, 0, z_face - 4]) cylinder(d = motor_ressalto_d + 0.8, h = 5);
        // berço do motor
        translate([cx, 0, z_face]) linear_extrude(luva_motor_h + 1) perfil_motor(folga);
        // fenda + furos M3 da braçadeira
        translate([cx - 0.75, 0, z_face]) cube([1.5, luva_r + 10, luva_motor_h + 1]);
        for (z = [z_face + luva_motor_h*0.3, z_face + luva_motor_h*0.75])
            translate([cx - 10, luva_r + 2.5, z]) rotate([0, 90, 0]) cylinder(d = 3.4, h = 20);
        // parafuso M4 de aperto na parede + porca embutida
        translate([xl - 1, 0, -enc_h/2]) rotate([0, 90, 0]) cylinder(d = 4.5, h = perna_e + 2);
        translate([xl - 1, 0, -enc_h/2]) rotate([0, 90, 0]) rotate([0, 0, 30]) hex(7 + folga, 3.5 + 1);
        // furo para travar o tubo (parafuso autoatarraxante 3 mm)
        translate([cx, 0, -enc_h/2]) rotate([0, 90, 0]) cylinder(d = 2.5, h = enc_de);
    }
}

// ---------- ACOPLAMENTO: eixo do motor <-> eixo sextavado ----------
module acoplamento() {
    furo_h = motor_eixo_comp - 1;
    h = acop_hex_prof + acop_sep + furo_h;
    zs = acop_hex_prof + acop_sep + furo_h/2;
    difference() {
        cylinder(d = acop_d, h = h);
        translate([0, 0, -1]) hex(eixo_ef + folga, acop_hex_prof + 1);
        translate([0, 0, acop_hex_prof + acop_sep]) cylinder(d = motor_eixo_d + 0.1, h = h, $fn = 24);
        // parafuso M3 (sem cabeça ou comum) + porca
        translate([0, 0, zs]) rotate([0, 90, 0]) cylinder(d = 3.3, h = acop_d);
        translate([3.6 - 1.4, -2.9, zs - 2.9]) cube([2.8, 20, 5.8]); // fenda da porca aberta para o lado
    }
}

// ---------- EIXO sextavado (impresso deitado) ----------
module eixo_reto(L) rotate([0, 90, 0]) rotate([0, 0, 30]) hex(eixo_ef, L);
module eixo() translate([0, 0, eixo_ef/2]) eixo_reto(seg_eixo);

module luva_eixo()
    difference() {
        cylinder(d = 12.6, h = 20);
        translate([0, 0, -1]) hex(eixo_ef + folga, 22);
    }

// ---------- TUBO-GUIA ----------
module tubo(L = seg_tubo)
    difference() {
        cylinder(d = tubo_de, h = L);
        translate([0, 0, -1]) cylinder(d = tubo_di, h = L + 2);
    }

module luva_tubo()
    difference() {
        cylinder(d = enc_de, h = 30);
        translate([0, 0, -1]) cylinder(d = enc_di, h = 32);
    }
module luva_tubo_batente()  // anel no meio onde os tubos encostam
    difference() { translate([0,0,14.5]) cylinder(d = enc_di + 0.1, h = 1); translate([0,0,14]) cylinder(d = tubo_di, h = 2); }

// ---------- ROTOR ----------
module rotor() {
    h = rotor_placa + rotor_pa_h;
    difference() {
        union() {
            cylinder(r = rotor_r, h = rotor_placa);
            cylinder(d = cubo_d, h = h);
            for (i = [0 : rotor_pas - 1]) rotate(i * 360 / rotor_pas)
                translate([cubo_d/2 - 1, -0.8, 0]) cube([rotor_r - cubo_d/2 + 1, 1.6, h]);
        }
        translate([0, 0, 1]) hex(eixo_ef + 0.2, h);
    }
}

// ---------- VOLUTA (corpo da bomba) ----------
module contorno_bomba() union() {
    circle(r = corpo_r);
    translate([xc_saida - bico_larg/2, 0]) square([bico_larg, corpo_r + 6]);
}
module perfil_voluta() union() {
    circle(r = r0);
    polygon([for (a = [0 : 4 : 360]) let(r = r0 + voluta_larg * a / 360) [r*cos(a), r*sin(a)]]);
    translate([r0, -1]) square([voluta_larg, corpo_r - 1]);
}
furos_ang = [120, 240, 330];
barb_d = mangueira_di + 1.0;
bico_furo = max(mangueira_di - 1.6, 2);
zc_bico = voluta_h / 2;

module farpa() {
    for (i = [0 : 1]) translate([0, 0, i * 5]) cylinder(d1 = barb_d, d2 = mangueira_di - 0.3, h = 5);
}

module voluta() {
    difference() {
        union() {
            linear_extrude(voluta_h) contorno_bomba();
            translate([xc_saida, corpo_r + 5.5, zc_bico]) rotate([-90, 0, 0]) farpa();
        }
        translate([0, 0, piso]) linear_extrude(camara_h + 1) perfil_voluta();
        // transição do canal retangular para o furo redondo do bico
        hull() {
            translate([r0, corpo_r - 3, piso]) cube([voluta_larg, 1, camara_h]);
            translate([xc_saida, corpo_r + 5, zc_bico]) rotate([-90, 0, 0]) cylinder(d = bico_furo, h = 1);
        }
        translate([xc_saida, corpo_r, zc_bico]) rotate([-90, 0, 0]) cylinder(d = bico_furo, h = 30, $fn = 24);
        // parafusos M3 de baixo para cima, porca embutida em baixo
        for (a = furos_ang) rotate(a) translate([furo_r, 0, 0]) {
            translate([0, 0, -1]) cylinder(d = 3.4, h = 50, $fn = 24);
            translate([0, 0, -1]) hex(5.5 + folga, 2.5 + 1);
        }
    }
}

// ---------- TAMPA com entrada de água + mancal inferior ----------
module tampa() {
    z_pernas = tampa_e; z_pisoE = tampa_e + pernas_h;
    difference() {
        union() {
            linear_extrude(tampa_e) contorno_bomba();
            translate([0, 0, z_pernas]) cylinder(d = enc_de, h = pernas_h + enc_piso + enc_prof);
        }
        // olho de entrada
        translate([0, 0, -1]) cylinder(d = olho_d, h = tampa_e + 2);
        // espaço interno + janelas por onde a água entra
        translate([0, 0, z_pernas - 0.1]) cylinder(d = enc_de - 6, h = pernas_h + 0.1);
        for (a = [0, 120, 240]) rotate(a + 60) translate([0, -7, z_pernas]) cube([30, 14, pernas_h - 1]);
        // mancal inferior
        translate([0, 0, z_pisoE - 1]) cylinder(d = bucha_d, h = enc_piso + 2);
        // encaixe do tubo
        translate([0, 0, z_pisoE + enc_piso]) cylinder(d = enc_di, h = enc_prof + 1);
        translate([0, 0, z_pisoE + enc_piso + enc_prof/2]) rotate([0, 90, 0]) cylinder(d = 2.5, h = enc_de);
        for (a = furos_ang) rotate(a) translate([furo_r, 0, -1]) cylinder(d = 3.4, h = 10, $fn = 24);
    }
}

// ============================ MONTAGEM ===============================
module motor_fantasma()
    color("silver") translate([cx, 0, z_face]) {
        linear_extrude(motor_comp) perfil_motor();
        translate([0, 0, -1.6]) cylinder(d = motor_ressalto_d, h = 1.6);
        translate([0, 0, -motor_eixo_comp]) cylinder(d = motor_eixo_d, h = motor_eixo_comp);
    }

module parede_fantasma()
    color("lightblue", 0.35) translate([-3, -60, -profundidade - 10]) cube([3, 120, profundidade + 10]);

module montagem() {
    zb = -profundidade;  // fundo da bomba
    parede_fantasma();
    color("orange") suporte();
    motor_fantasma();
    color("gold") translate([cx, 0, z_acop]) acoplamento();
    color("dimgray") translate([cx, 0, eixo_base]) hex(eixo_ef, eixo_comp);
    color("white", 0.6) translate([cx, 0, -tubo_comp]) tubo(tubo_comp);
    color("red") translate([cx, 0, zb + piso + 1]) rotor();
    color("seagreen") translate([cx, 0, zb]) voluta();
    color("mediumseagreen") translate([cx, 0, zb + voluta_h]) tampa();
}

// ============================ SAÍDA ==================================
if (peca == "montagem")    montagem();
if (peca == "suporte")     translate([0, 0, enc_h]) suporte();
if (peca == "acoplamento") acoplamento();
if (peca == "eixo")        eixo();
if (peca == "luva_eixo")   luva_eixo();
if (peca == "tubo")        tubo();
if (peca == "luva_tubo")   { luva_tubo(); luva_tubo_batente(); }
if (peca == "rotor")       rotor();
if (peca == "voluta")      voluta();
if (peca == "tampa")       tampa();
