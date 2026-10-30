# Documento explicativo - Proyecto Intermedio

---

#### Adrián Isasi Martínez

NOTA: El documento está escrito en *markdown* (.md), es un formato légible sin previsualizador, pero hay aplicaciones que muestran el texto más bonito.


## Instrucciones para ejecutar
No hay un juego todavía, mi juego usará principalmente rigidbodies, el proyecto solo contiene el código de las prácticas. Para ejecutarlo, se debe abrir el proyecto con VS2022 (puede que funcione con otras versiones de VS, pero no lo he probado), y presionar la flechita verde para ejecutar la aplicación. Se puede seleccionar otro perfil de compilación.
> [!WARNING] Solo se ejecuta en DEBUG, intenté averiguar porqué no compila en RELEASE, pero no lo conseguí.

## Teclas disponibles
Cada escena es una demo de dicha práctica. A continuación se describen las teclas disponibles en cada escena.

> [!NOTE] '[<tecla>]' significa que hay que presionar la tecla con el carácter <tecla>.

*Cambiar de escena*:
- Escena 1 (demo de práctica 1): [1]
- Escena 2 (demo de práctica 2): [2]
- Escena 3 (demo de práctica 3): [3]
- Escena 4 (demo de práctica 4): [4]

*Teclas de la escena 1*:
- Bala de cañón: [c]
- Bala de tanque: [v]
- Bala de pistola: [b]
- Aumentar la masa (real) del siguiente proyectil: [y]
- Disminuir la masa (real) del siguiente proyectil: [h] 
- Aumentar la velocidad (real) del siguiente proyectil: [u]
- Disminuir la velocidad (real) del siguiente proyectil: [j]

*Teclas de la escena 2*:
- Mostrar ayuda de teclas por consola: [h]
- Generar 500 partículas: [g]
- Cambiar la posición del generador: [i] [j] [k] [l]
- Activar o desactivar la emisión de partículas: [e]
- Cambiar la forma de generación: (Punto) [c], (Esfera) [v], (Cubo) [b]
- Aumentar o disminuir la generación por segundo: [+] [-]
- Cambiar el tipo de velocidad: (Independiente) [t], (Radial) [r]
- Poner/Quitar velocidad de prueba ({0, 10, 0}) (solo en velocidad independiente): (poner) [y], (quitar) [u]
- Activar/Desactivar gravedad: [z] [x]

*Teclas de la escena 3*:
- Activar o desactivar la emisión de partículas: [e]
- Aumentar o disminuir la generación de partículas por segundo: [+] [-]
- Activar el generador de gravedad: [g]
- Activar el generador de viento: [r]
- Activar el generador de tornado: [t]
- Activar el generador de explosiones: [x]

*Teclas de la escena 4*: Esta escena no es parte de la entrega, pero bueno, las teclas son las siguientes:
- Aplicar fuerza horizontal a la partícula blanca: [f]
- Aumentar o disminuir la constante K: [k] [j]


## Requisitos de programación

**Requisito A.1)** ->  "Particle.cpp" 49 - 94

**Requisito A.2)** -> "Particle.h" 52 atributo de la inversa de la masa, "P1_Scene.h" se puede cambiar la masa para ver los cambios en la simulación de proyectiles. También se juega con la masa en la escena "P3_Scene.h" (fuerzas).

**Requisito A.3)** -> "Particle.h" 48 - 49 métodos para aplicar o resetear fuerzas. "Particle.cpp" 49 - 94 aplicación de las fuerzas en los distintos integradores

**Requisito B)** -> "Projectile.h" y "Projectile.cpp", en la escena 1 ("P1_Scene.h") se puede ver que hay 3 tipos de proyectiles y la posibilidad de modificar sus propiedades con 'offsets'.

**Requisito C)** -> "ParticleSystem.h" y "ParticleSystem.cpp", básicamente es un contenedor de partículas ("Particle.h"). Para mantener una abstracción limpia, NO es un contenedor de fuerzas, y NO gestiona las fuerzas, eso se hace por separado para mantener la modularidad del código. Contiene y mantiene un conjunto de partículas, y expone una interfaz segura al mismo. Los generadores de fuerzas usan esta interfaz para aplicar la fuerza a las partículas.

**Requisito D.1)** -> "ParticleGenerator.h" 7 - 24,  La configuración de generación del generador está contenida en el struct *ParticleGeneratorConfig* que define los atributos usados para generar partículas por el generador. Previamente usaba una partícula como modelo, pero lo cambie a campos libres porque la partícula no tenía todos los atributos necesarios y era mucho menos legible qué era configurable y qué no.

**Requisito D.2)** -> "ParticleGenerator.h" 12 - 13, permite configurar parámetros de generación (como la posición en este caso), y "ParticleGenerator.cpp" 13 - 37 función de ayuda que se usa para conseguir un punto aleatorio según la configuración del usuario. Si se quiere generar en un cubo, se usa obligatoriamente una Gaussiana, una esfera usa una Normal. El resto de parámetros se puede escoger qué tipo de función usar al crear el objeto generador, pasando como parámetro de construcción la función de números aleatorios ("ParticleGenerator.h" 29)

**Requisito D.3)** -> Yo he hecho una implementación distinta (en mi opinión es más claro y desacoplado). En vez de que un sistema de partículas contenga un generador, uno puede indicarle a un generador cualquiera en qué sistema de partículas debe generar partículas. Esto hace que la dependencia entre ambos sea unidireccional y no bidireccional. Además permite que un generador cambie el sistema en el que genera partículas, o la forma en las que las genera sin problemas de referencias. Un ejemplo de implementación es "P2_Scene.h", donde se puede configurar el generador en tiempo real mientras genera partículas para ver el resultado de los cambios.

**Requisito E.1)** -> "ForceGenerator.h" 22 - 34 (el generador de gravedad). La única función del generador de fuerzas es, dada una párticula y una configuración, aplicar la fuerza correcta a dicha partícula. La fuerza aplicada a cada partícula es calculada por separado, pues consulta las propiedades de cada partícula. Me parece muy importante aclarar que la partícula NO sabe nada de los generadores de fuerzas, esto es importante que sea así porque queremos desacoplar la lógica de las partículas de los generadores de fuerzas.

**Requisito E.2)** -> "P3_Scene.h" 60 - 78 Una partícula puede verse afectada por más de un generador de fuerzas si se ordena que distintos generadores de fuerzas afecten al mismo sistema de partículas (o la misma partícula).

**Requisito E.3)** -> "ForceGenerator.h" o "P3_Scene.h" Los generadores de fuerzas no guardan ninguna referencia a ningún sistema. Solo saben aplicar una fuerza a una partícula dada (o sistema, usando los iteradores). En "P3_Scene.h" se puede observar que distintos sistemas de partículas reciben fuerzas totalmente distintas, esto lo define el código usuario de las clases con total modularidad.

**Requisito E.4)** -> "P3_Scene.h" 60 - 78 Se puede observar que se han implementado las fuerzas del viento, tornados, gravedad, (para flotación mejor ver P4_Scene aunque no sea de esta entrega), y explosiones.


## Requisitos mínimos de ejecución
1. "P1_Scene.h", la escena 1 permite disparar distintos proyectiles con distintas propiedades y probar con distintas configuraciones.
2. "P3_Scene.h", Se puede activar cada generador con teclas distintas, pudiendo tener activos todos, algunos o ningún generador.