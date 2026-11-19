// =====================================================================
//  Distribuidor em "T" para regar 2 vasos ao mesmo tempo
//  - Barra de 30 cm, tubo de 9 mm com parede de 2 mm (furo de 5 mm)
//  - Entrada no centro para o tubinho que vem da bomba
//  - Pontas dobradas para baixo: entram no vaso e seguram a barra
//
//  Impresso em 3 peças (30 cm não cabe na maioria das mesas):
//    1x "centro" (o T com a entrada) + 2x "braco" (iguais), colados
//
//  openscad -D 'peca="braco"' -o stl/distribuidor_braco.stl distribuidor.scad
// =====================================================================

/* [Peça] */
peca = "montagem"; // [montagem, centro, braco]

/* [Medidas] */
comprimento  = 300;  // comprimento total, de ponta a ponta
tubo_de      = 9;    // diâmetro externo do tubo
parede       = 2;    // espessura da parede (furo = tubo_de - 2*parede)
entrada_de   = 3.85; // diâmetro EXTERNO do tubinho que vem da bomba
entrada_prof = 10;   // quanto o tubinho entra
dobra_raio   = 10;   // raio da curva da dobra (na linha do centro)
perna        = 30;   // comprimento da parte reta que desce para dentro do vaso

/* [Impressão] */
folga = 0.3;
achatar = 0.6;       // achata a base para grudar melhor na mesa
$fn = 64;

// ---------------- derivados ----------------
tubo_di  = tubo_de - 2*parede;
enc_prof = 12;                    // quanto o braço entra no centro
enc_di   = tubo_de + folga;
centro_de = enc_di + 2*parede;
centro_meio = 10;                 // parte maciça entre os dois encaixes
centro_comp = 2*enc_prof + centro_meio;
braco_ext = (comprimento - centro_comp)/2 + enc_prof;  // do fim encaixado até a face de fora da perna
reta = braco_ext - dobra_raio - tubo_de/2;             // trecho reto horizontal do braço
torre_h = centro_de/2 + entrada_prof + 2;              // altura da entrada a partir do eixo

echo(str("== braço: reta ", reta, " mm, total ", braco_ext, " mm de largura x ",
         dobra_raio + perna + tubo_de/2, " mm de altura"));

// ---------------- BRAÇO (desenhado deitado, como imprime) ----------------
// Eixo do tubo no plano z = tubo_de/2; o tubo corre em +x e dobra para -y.
module caminho(d, extra = 0) {
    // reto
    translate([-extra, 0, 0]) rotate([0, 90, 0]) cylinder(d = d, h = reta + extra + 0.01);
    // curva de 90°
    translate([reta, -dobra_raio, 0]) rotate_extrude(angle = 90) translate([dobra_raio, 0]) circle(d = d);
    // perna
    translate([reta + dobra_raio, -dobra_raio + 0.01, 0]) rotate([90, 0, 0]) cylinder(d = d, h = perna + extra);
}

module braco() {
    difference() {
        translate([0, 0, tubo_de/2]) caminho(tubo_de);
        translate([0, 0, tubo_de/2]) caminho(tubo_di, 1);
        translate([-50, -500, -1]) cube([1000, 1000, 1 + achatar]);   // base plana
    }
}

// ---------------- CENTRO (T com a entrada) ----------------
// Desenhado em pé, como imprime: barra deitada em x, entrada para cima.
module centro() {
    zc = centro_de/2;
    difference() {
        union() {
            translate([-centro_comp/2, 0, zc]) rotate([0, 90, 0]) cylinder(d = centro_de, h = centro_comp);
            // torre da entrada
            cylinder(d = entrada_de + 2*parede + 0.15, h = zc + torre_h);
            // reforço entre a torre e a barra
            translate([0, 0, zc]) cylinder(d1 = centro_de, d2 = entrada_de + 2*parede, h = centro_de/2 + 2);
        }
        // encaixes dos braços
        for (s = [-1, 1]) translate([s > 0 ? centro_comp/2 - enc_prof : -centro_comp/2 - 1, 0, zc])
            rotate([0, 90, 0]) cylinder(d = enc_di, h = enc_prof + 1);
        // canal interno entre os encaixes
        translate([-centro_comp/2, 0, zc]) rotate([0, 90, 0]) cylinder(d = tubo_di, h = centro_comp);
        // entrada do tubinho + canal até a barra
        translate([0, 0, zc + torre_h - entrada_prof]) cylinder(d = entrada_de + 0.15, h = entrada_prof + 1, $fn = 32);
        translate([0, 0, zc]) cylinder(d = min(entrada_de - 1, tubo_di), h = torre_h, $fn = 32);
        // base plana
        translate([-100, -100, -1]) cube([200, 200, 1 + achatar]);
    }
}

// ---------------- MONTAGEM (posição de uso: pernas para baixo) ----------------
module montagem() {
    zc = centro_de/2;
    color("mediumseagreen") translate([0, 0, -zc]) centro();
    for (s = [-1, 1]) color("seagreen")
        mirror([s < 0 ? 1 : 0, 0, 0])
        translate([centro_comp/2 - enc_prof, 0, 0]) rotate([90, 0, 0]) translate([0, 0, -tubo_de/2]) braco();
}

if (peca == "montagem") montagem();
if (peca == "centro")   centro();
if (peca == "braco")    braco();
