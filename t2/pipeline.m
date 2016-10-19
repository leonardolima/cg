# this is a script file
1;

clear all;

################################################################################
# Vertices do modelo (cubo) no espaco do objeto.
################################################################################
v_obj(1).c = [-1; -1; -1; 1];
v_obj(2).c = [-1; -1;  1; 1];
v_obj(3).c = [ 1; -1; -1; 1];
v_obj(4).c = [ 1; -1;  1; 1];
v_obj(5).c = [-1;  1; -1; 1];
v_obj(6).c = [-1;  1;  1; 1];
v_obj(7).c = [ 1;  1; -1; 1];
v_obj(8).c = [ 1;  1;  1; 1];

################################################################################
# Matriz model (escala isotropica com fator 2)
################################################################################
M_model = [ 2 0 0 0; \
            0 2 0 0; \
            0 0 2 0; \
            0 0 0 1];

################################################################################
# Parametros da camera
################################################################################
camera_pos    = [0; 0; 5];  # Posicao da camera no universo.
camera_lookat = [0; 0; 0];  # Ponto para onde a camera esta olhando.
camera_up     = [0; 1; 0];  # 'up' da camera no espaco do universo.

################################################################################
# Calculo do sistema ortonormal gerado a partir dos parametros da camera
################################################################################
z_camera = (camera_pos - camera_lookat) / norm(camera_pos - camera_lookat);
x_camera = cross(camera_up, z_camera) / norm(cross(camera_up, z_camera));
y_camera = cross(z_camera, x_camera);

################################################################################
# Construcao da matriz view
################################################################################
Bt = [x_camera(1), x_camera(2), x_camera(3), 0; \
      y_camera(1), y_camera(2), y_camera(3), 0; \
      z_camera(1), z_camera(2), z_camera(3), 0; \
      0, 0, 0, 1];

T = [1, 0, 0, -camera_pos(1); \
     0, 1, 0, -camera_pos(2); \
     0, 0, 1, -camera_pos(3); \
     0, 0, 0, 1];

M_view = Bt * T;

################################################################################
# Construcao da matriz Modelview
################################################################################
M_modelview = M_view * M_model;

################################################################################
# Construcao da matriz de Projecao, com d = 1
################################################################################
d = 1;

M_projection = [1 0 0 0; \
                0 1 0 0; \
                0 0 1 d; \
                0 0 -1/d 0];

################################################################################
# Construcao da matriz Modelviewprojection
################################################################################
M_modelviewprojection = M_projection * M_modelview;

################################################################################
# Aplicacao da matriz Modelviewprojection sobre os vertices no espaco do objeto
################################################################################
for i = 1 : 8
    v_clip(i).c = M_modelviewprojection * v_obj(i).c;
endfor

################################################################################
# Homogeneizacao (divisao por W).
# Este passo leva os vertices normalmente para o espaço canonico. 
# Neste caso, como a matriz de projecao eh muito simples, o resultado
# da homogeneizacao sao os vertices em um espaço que nao eh exatamente o 
# canonico. Porem, apesar de nao ser o espaco canonico, a distorcao perspectiva
# estara presente.
################################################################################
for i = 1 : 8
    v_canonic(i).c = v_clip(i).c / v_clip(i).c(4);
endfor

################################################################################
# Conversão de coordenadas do espaço canônico para o espaço de tela.
################################################################################

S1 = [1 0 0 0; \
      0 -1 0 0; \
      0 0 1 0; \
      0 0 0 1];

T = [1 0 0 1; \
     0 1 0 1; \
     0 0 1 0; \
     0 0 0 1];

w = 512
h = 512

S2 = [(w-1)/2 0 0 0; \
      0 (h-1)/2 0 0; \
      0 0 1 0; \
      0 0 0 1];

M_viewport = S2 * T * S1;

for i = 1 : 8
    v_screen(i).c = M_viewport * v_canonic(i).c; 
endfor

v_screen(1).c
v_screen(2).c
v_screen(3).c
v_screen(4).c
v_screen(5).c
v_screen(6).c
v_screen(7).c
v_screen(8).c


