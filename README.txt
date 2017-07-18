Piersicanu Radu 332CB

Utilizare aplicatie:
	-miscare bila generatoare de picaturi: W,A,S,D,Q,E
	-miscare camera: click dreapta + W,A,S,D,Q,E
	-miscare lumina: click stanga + W,A,S,D,Q,E
	-generare picaturi: Space
	-slow-motion: sageata stanga
	-fast-forward: sageata dreapta

Implmentare:
	Waves.cpp
		-Init() -> se initializeaza variabile si se creaza plasa poligonala
		-Update() -> randare si updatare obiecte
		-RenderSimpleMesh() -> transmiterea informatiilor la shadere
		-OnInputUpdate() -> miscare
		-OnKeyPress() -> taste slow-motion/fast-forward si generare picaturi
	Sphere.cpp
		-contine vectori cu informatiile undelor
		-collision() -> atunci cand o picatura atinge apa se genereaza un nou val,
		 		marimea picaturii se injumatateste si se schimba viteza picaturii
				in directia opusa
		-erase() -> functie apelata atunci cand amplitudinea unui val ajunge la 0
		-wave_update() -> atenuare val si modificare distanta fata de centru
		-update() -> modificare viteza picatura si pozitia acesteia in functie de
				viteza
		-init() -> initializeaza valori pentru picatura
		-applyForce() -> aplica o forta asupra picaturii
		-solve() -> apeleaza applyForce()
		-operate() -> apeleaza functii pentru a simula picatura si valurile
	VertexShader
		-primeste informatii de la CPU despre valuri
		-calculeaza inaltimea si normala fiecarui vertex
	FragmentShader
		-iluminare folosind Phong