vtk_module(pqPython
  DEPENDS
    vtkPythonInterpreter
    vtkWrappingPythonCore
    pqCore
  PRIVATE_DEPENDS
    Pygments
  EXCLUDE_FROM_WRAPPING
  TEST_LABELS
    PARAVIEW
)
