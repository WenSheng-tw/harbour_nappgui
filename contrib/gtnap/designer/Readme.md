# NAppGUI designer and GTNAP-Forms

NAppGUI Designer es una herramienta visual para dise�ar interfaces de usuario (formularios) de forma gr�fica e interactiva. Estos formularios se guardar�n en archivos que podr�n ser cargados en tiempo de ejecuci�n desde Harbour, utilizando GTNAP. Se ha desarrollado utilizando NAppGUI-SDK y los formularios que crea tambi�n utilizan NAppGUI para .... dentro de la aplicaci�n final (https://nappgui.com).

## Vista general de NApDesigner

En principio, la apariencia de la aplicaci�n es muy parecida a la de otras herramientas similares (QTDesigner, por ejemplo). En la parte central tendremos el �rea de dise�o donde visualizamos el formulario en construcci�n. A la izquierda disponemos de una lista de archivos y un selector de widgets. A la derecha tenemos el inspector de objetos y el editor de propiedades. En la parte superior veremos la t�pica barra de herramientas para la gesti�n de archivos y una barra de estado en la parte inferior.

![designer](./images/designer.png)

## Compilar NApDesigner

Designer se distribuye como parte de GTNAP, por lo que no hay que hacer nada especial para compilarla, tan solo correr el script de build en `contrib\gtnap`. Mas informaci�n en [Build GTNAP](../Readme.md#build-gtnap)

```
:: Windows MinGW
build.bat -b [Debug|Release] -comp mingw64

:: Linux / macOS
bash ./build.sh -b [Debug|Release]
```
La aplicaci�n la tendremos en `build/[Debug|Release]/bin/napdesign`.

## Abrir carpeta de proyecto

La primera vez que arranca la aplicaci�n tendremos un �rea de dibujo en blanco y todos los botones apagados. Lo primero que tenemos que hacer es pulsar sobre el icono carpeta (??) y seleccionar un directorio de proyecto. NApDesigner permite editar simult�neamente todos los formularios de una carpeta determinada. La carpeta actual la podemos ver situando el rat�n encima del icono. Haciendo clic sobre cualquier archivo, lo deseccionaremos y veremos en el �rea de dibujo.

![openfolder](./images/openfolder.png)

## Crear un nuevo formulario

Una vez abierta la carpeta del proyecto, pulsando el bot�n (?) crearemos un nuevo formulario. Lo �nico que veremos es un peque�o rect�ngulo en el �rea de dibujo que representa un layout de una sola celda.

![newform](./images/newform.png)

NAppGUI basa la composici�n de interfaces de usuario en el concepto de Layout (GridLayout en QtDesigner) que dividir� el espacio en (ncols x nrows) celdas. La diferencia con otras herramientas es que NAppGUI **no soporta elementos flotantes**. Todos los widgets deben estar dentro de una celda en un layout. Como veremos a continuaci�n, la principal ventaja de esto que **no es necesario establecer el marco** (posici�n y tama�o) de cada elemento, ya que ser� calculado autom�ticamente por NAppGUI en funci�n del API nativo (Win32, GTK, Cocoa).