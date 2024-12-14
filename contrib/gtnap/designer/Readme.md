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

### Subdivisi�n del espacio. A�adir celdas

A partir de aqu� deberemos sub-dividir esta primera celda utilizando el componente _Grid Layout_ del selector de widgets. En funci�n del dise�o que persigamos para nuestro panel haremos unas u otras subdivisiones.

* Selecciona _Grid Layout_, haz clic sobre la celda. Aparecer� un di�logo, donde seleccionados **Columns: 1, Rows: 2, [OK]**.

    ![subdiv1](./images/subdivision1.png)

* Vemos que en el _Object Inspector_ se va formando una jerarqu�a (camino) de Layouts y Cells.

    ![inspect1](./images/obinspect1.png)

    * **layout0:** Layout principal compuesto de (1x1) celda.
    * **cell0:** Celda [0,0] de layout0.
    * **layout1:** Layout de (1x2) celdas ubicado en cell0 (posici�n [0,0] de layout0).
    * **cell2:** Celda (0,0) de layout1, por el momento vac�a.
    * Profundizaremos m�s adelante en el _Object Inspector_. Por el momento, vamos observando como cambia el panel a medida que realizamos subdivisiones.

* Manteniendo _Grid Layout_ en el selector de widgets, hacemos click sobre la celda superior y seleccionamos: **Columns: 2, Rows: 1, [OK]**.

    ![subdiv2](./images/subdivision2.png)

* En la celda superior izquierda, creamos un grid de 2 columnas y 9 filas.

    ![subdiv3](./images/subdivision3.png)

* En la celda de la derecha 1 columna y 4 filas.

    ![subdiv4](./images/subdivision4.png)

* Y, por �ltimo, en la celda inferior, 2 columnas y 1 fila. Con esto, hemos alcanzado la configuraci�n de celdas necesaria para nuestro formulario. Vamos a empezar a insertar contenido.

    ![subdiv5](./images/subdivision5.png)

### A�adir Widgets

* Selecciona _Label_ en el selector de widgets. Empezando por la celda superior izquierda, vamos a crear etiquetas para las nueve celdas de la izquierda: `First Name`, `Last Name`, `Address`, `City`, `Phone number`, `User`, `Pass`, `Bank account` y `Credit card`. Ver�s como los textos m�s anchos desplazar�n a la derecha al resto de celdas. Esto es un efecto de la maquetaci�n autom�tica que realizar NAppGUI.

    ![widgets1](./images/widgets1.png)

* Selecciona _Editbox_ en el selector de widgets. Vamos a a�adir un componente para cada celda a la derecha de los textos, excepto para `Bank account` y `Credit card`. Por el momento, utiliza las opciones por defecto al crear los Editbox.

    ![widgets2](./images/widgets2.png)

* Para el caso del `Bank account` y `Credit card` queremos separar la entrada en diferentes Editbox. Vuelve a seleccionar _Grid layout_ en el selector de widgets y crea 4 columnas y 1 fila para el `Bank account` y 5 columnas para el `Credit card`. Observar�s que el formulario se va expandiendo horizontalmente, por el momento no te preocupes por esto.

    ![widgets3](./images/widgets3.png)

* A�ade un _Editbox_ por cada celda del `Bank account` y `Credit card`.

    ![widgets4](./images/widgets4.png)

* Selecciona _Button_ en el selector de widgets y a�ade dos botones `[OK]` y `[Cancel]` en las dos celdas inferiores.

    ![widgets5](./images/widgets5.png)

* Por �ltimo selecciona _Checkbox_ en el selector de widgets y crea 4 checks en las celdas restantes de la derecha:
    * `Add mail list`.
    * `Secure password`.
    * `Show alerts`.
    * `Connect bank account`.

    ![widgets6](./images/widgets6.png)

* Para concluir pulsa el bot�n (??) _Simulate current form_ para comprobar el funcionamiento de nuestro formulario hasta ahora.

    ![simulate1](./images/simulate1.png)

