rem copy to app
SET prjoctName=MCI3R_AMC01_ARM
SET projectVersion=_V.003.0.0.0

copy /Y ..\app\include\lynx_types.h               ..\..\..\MCI3R_AMC01_IF_ARM\
copy /Y ..\module\*.h                             ..\..\..\MCI3R_AMC01_IF_ARM\
copy /Y liblynx.a                                 ..\..\..\MCI3R_AMC01_IF_ARM\
copy /Y ..\lynx_common\*.h                        ..\..\..\MCI3R_AMC01_IF_ARM\
copy /Y ..\lynx_common\*.s                        ..\..\..\MCI3R_AMC01_IF_ARM\
copy /Y ..\lynx_common\*.ld                       ..\..\..\MCI3R_AMC01_IF_ARM\
copy /Y ..\lynx_common\kcg_libs.c                 ..\..\..\MCI3R_AMC01_IF_ARM\

del /f /s /q ..\bin\*.*
copy /Y ..\Debug\%prjoctName%.bin                 ..\Debug\%prjoctName%%projectVersion%.bin