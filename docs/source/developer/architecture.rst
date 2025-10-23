###################
Architecture of WTF
###################

****
FAQs
****

- There's a lot of repetition between ``FloatHolder`` and ``FloatViewHolder``,
  why can't we combine them?

  - In hindsight, we could define a class ``FloatHolderAPI<T>`` that is 
    templated on the type of ``Float`` that will hold it. ``FloatModel`` would 
    always derive from ``FloatHolderAPI<Float>`` whereas ``FloatViewModel`` 
    would derive from ``FloatHolderAPI<Float>`` or 
    ``FloatHolderAPI<const Float>`` depending on the const-ness of the type 
    argument to ``FloatViewModel``. The holder classes were already written
    when this realization occurred, so it was not worth the effort to refactor
    them.