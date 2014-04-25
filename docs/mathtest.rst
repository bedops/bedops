===================================================================================
Math markup tests
===================================================================================

Inline test: Since Pythagoras, we know that :math:`a^2 + b^2 = c^2`.

Displayed tests:

.. math::

   (a + b)^2 = a^2 + 2ab + b^2

   (a - b)^2 = a^2 - 2ab + b^2

.. math::
   :nowrap:

   \begin{eqnarray}
      y    & = & ax^2 + bx + c \\
      f(x) & = & x^2 + 2xy + y^2
   \end{eqnarray}

A cross-referenceable equation (one that could be dropped into multiple locations in same doc):

.. math:: e^{i\pi} + 1 = 0
   :label: euler

Euler's identity, equation :eq:`euler`, was elected one of the most
beautiful mathematical formulas.
