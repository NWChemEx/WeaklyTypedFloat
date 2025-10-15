#pragma once
#include <wtf/concepts/floating_point.hpp>
#include <wtf/detail_/downcaster.hpp>

namespace wtf::detail_ {

/** @brief Tracks the unwrapped variants as we resolve them.
 *
 *  @tparam VariantTuple A std::tuple of std::variant types. Each variant holds
 *                       pointers to type-restored models.
 *
 *  As dispatch recurses it will downcast holders to models. The downcasted
 *  Models are then stored in a tuple as they are found. This class wraps the
 *  process of "expanding" the tuple of variants and when the time comes to run
 *  the callable this class is capable of invoking the callable with the Models.
 *
 *  This class is used by:
 *  1. Create the first instance once the first Model is found.
 *  2. Call add_variant to get the second instance once the second Model is
 *     found.
 *  3. Repeat step 2 until all Models are found.
 *  4. Call `run` with the user-provided callable. `run` will return the result
 *     of the user's callable.
 */
template<typename VariantTuple>
class DispatchHelper {
public:
    /** @brief Creates an instance capable of visiting @p variants.
     *
     *  @tparam VariantTuple2 The type of the tuple of variants to visit.
     *                        VariantTuple2 must be implicitly convertible to
     *                        VariantTuple.
     *
     *  This ctor perfectly forwards @p variants into the member variable.
     *
     *  @param[in] variants The tuple of variants to visit.
     *
     *  @throws ??? Any exception thrown by the copy or move constructor of
     *              @p variants. Same throw guarantee as the copy or move
     *              ctor of @p variants.
     */
    template<typename VariantTuple2>
        requires std::is_convertible_v<VariantTuple2, VariantTuple>
    explicit DispatchHelper(VariantTuple2&& variants) :
      m_variants_(std::forward<VariantTuple2>(variants)) {}

    /** @brief Creates a new DispatchHelper holding the variants in *this plus
     *         @p variant.
     *
     *  @tparam VariantType The type of the variant to add to the tuple.
     *
     *  This method is called when we have worked out the type of a new model.
     *  Once we have the new model we need to create a new DispatchHelper that
     *  contains the models in *this plus the new model. This method wraps that
     *  process. The new variant is appended to tuple of variants stored in
     *  *this.
     *
     *  @param[in] variant The new variant to add to the tuple.
     *
     *  @return A new DispatchHelper instance holding the variants in *this plus
     *          @p variant.
     *
     *  @throws ??? Any exception thrown by the copy or move constructor of the
     *              variants. Same throw guarantee as the copy or move ctor of
     *              the variants.
     */
    template<typename VariantType>
    auto add_variant(VariantType&& variant) {
        auto as_tuple     = std::make_tuple(std::forward<VariantType>(variant));
        auto catted_tuple = std::tuple_cat(m_variants_, std::move(as_tuple));
        using new_tuple_type = decltype(catted_tuple);
        return DispatchHelper<new_tuple_type>(std::move(catted_tuple));
    }

    /** @brief Initiates the dispatch.
     *
     *  @tparam FxnType The type of the callable to invoke.
     *
     *  This method expands the tuple of variants stored in *this and calls the
     *  user-provided callable with the models stored in the variants. The
     *  user-provided callable must have an API so that it can be called with
     *  any combination of the models stored in the variants, else this will
     *  fail to compile.
     *
     *  The signature of each overload should be something like:
     *  `R(const Model<Type1>&, Model<Type2>&, ...)` where `R` is the
     *  type returned from the callable and `Type1`, `Type2`, ... are the types
     *  each Model wraps. Note that models are taken by reference and can be
     *  const or non-const as long as the holders are mutable. If a holder is
     *  const, the corresponding model must be const as well.
     *
     *  @param[in] fxn The callable to invoke.
     *
     *  @return The return value returned by the callable.
     *
     *  @throws ??? Any exception thrown by the callable. Same throw guarantee
     *              as the callable.
     */
    template<typename FxnType>
    auto run(FxnType&& fxn) {
        return std::apply(
          [&](auto&&... args) {
              return run_impl_(std::forward<FxnType>(fxn),
                               std::forward<decltype(args)>(args)...);
          },
          m_variants_);
    }

private:
    /** @brief Implements run, by calling visit on the series of provided
     *         variants.
     *
     *  @tparam FxnType The type of the callable to invoke.
     *  @tparam Args The types of the variants to visit.
     *
     *  This method wraps the call to std::visit, and dereferences the
     *  pointers before passing them to the user-provided callable.
     *
     *  @param[in] fxn The callable to invoke.
     *  @param[in] args The variants to visit.
     *
     *  @return The return value of the callable.
     *
     *  @throws ??? Any exception thrown by the callable. Same throw guarantee
     *              as the callable.
     *
     */
    template<typename FxnType, typename... Args>
    auto run_impl_(FxnType&& fxn, Args&&... args) {
        auto deref_pointers = [&](auto&&... args) {
            return fxn(*std::forward<decltype(args)>(args)...);
        };

        return std::visit(deref_pointers, std::forward<Args>(args)...);
    }

    /// These are the variants to visit.
    VariantTuple m_variants_;
};

/** @brief Factors the common parts of dispatch_impl's branches into one place.
 *
 *  @tparam ModelType The template template parameter for the Model type to
 *                    downcast to. Must be provided by the user.
 *  @tparam TypeTuple A std::tuple of floating point types that ModelType can
 *                    wrap. Must be provided by the user.
 *  @tparam FxnType The type of the callable to invoke. Will be inferred by the
 *                  compiler.
 *  @tparam VariantTuple A std::tuple of std::variant types. Each variant holds
 *                      a pointer to a type-restored model. Will be inferred by
 *                      the compiler.
 *  @tparam HolderTypes The types of the remaining holders to downcast. Will be
 *                     inferred by the compiler.
 *
 *  This function wraps branching for the recursion termination condition vs.
 *  continuing the recursion process. If there are more holders to downcast
 *  this function calls dispatch_impl to continue the recursion. If there are
 *  not any more holders to downcast this function invokes the supplied
 *  function and returns the result.
 *
 *  @param[in] fxn The callable to invoke.
 *  @param[in] helper The DispatchHelper instance holding the variants found so
 *                    far.
 *  @param[in] other_holders The remaining holders to downcast.
 *
 *  @return The return value of the callable.
 *
 *  @throws ??? Any exception thrown by the callable or by dispatch_impl. Same
 *              throw guarantee.
 */
template<template<concepts::FloatingPoint> typename ModelType,
         typename TypeTuple, typename FxnType, typename VariantTuple,
         typename... HolderTypes>
auto dispatch_impl_common(FxnType&& fxn, DispatchHelper<VariantTuple> helper,
                          HolderTypes&&... other_holders) {
    constexpr auto n_other_holders = sizeof...(other_holders);
    if constexpr(n_other_holders > 0) {
        return dispatch_impl<ModelType, TypeTuple>(
          std::forward<FxnType>(fxn), std::move(helper),
          std::forward<HolderTypes>(other_holders)...);
    } else {
        return helper.run(std::forward<FxnType>(fxn));
    }
}

/** @brief Wraps the recursion process for dispatching.
 *
 *  @tparam ModelType The template template parameter for the Model type to
 *                    downcast to. Must be provided by the user.
 *  @tparam TypeTuple A std::tuple of floating point types that ModelType can
 *                    wrap. Must be provided by the user.
 *  @tparam FxnType The type of the callable to invoke. Will be inferred by the
 *                  compiler.
 *  @tparam VariantTuple A std::tuple of std::variant types. Each variant holds
 *                       a pointer to a type-restored model. Will be inferred by
 *                       the compiler.
 *  @tparam HolderType The type of the holder this level of recursion will
 *                     downcast. Will be inferred by the compiler.
 *  @tparam HolderTypes The types of the remaining holders to downcast. Will be
 *                      inferred by the compiler.
 *
 *  This function downcasts @p h to a ModelType wrapping one of the types in
 *  TypeTuple. The downcasted model is then added to @p helper. If there are
 *  more holders to downcast, this function recurses by calling itself. If not,
 *  this function invokes the user-provided callable with the models stored
 *  in @p helper and returns the result.
 *
 *  @param[in] fxn The callable to invoke.
 *  @param[in] helper The DispatchHelper instance holding the variants found so
 *                    far.
 *  @param[in] h The holder to downcast at this level of recursion.
 *  @param[in] other_holders The remaining holders to downcast.
 *
 *  @return The return value of the callable.
 *
 *  @throws ??? Any exception thrown by the callable or by dispatch_impl_common.
 *              Same throw guarantee as the callable or dispatch_impl_common.
 */
template<template<concepts::FloatingPoint> typename ModelType,
         typename TypeTuple, typename FxnType, typename VariantTuple,
         typename HolderType, typename... HolderTypes>
auto dispatch_impl(FxnType&& fxn, DispatchHelper<VariantTuple> helper,
                   HolderType&& h, HolderTypes&&... other_holders) {
    const bool is_const = h.is_const();

    // Because we must call the correct downcaster based on cv-qualifiers that
    // are only knowable at runtime, and because those downcasters all have
    // different return types (and thus signatures) we must explicitly branch.

    if(is_const) {
        auto v =
          const_downcaster<ModelType, TypeTuple>(std::forward<HolderType>(h));
        auto new_helper = helper.add_variant(std::move(v));
        return dispatch_impl_common<ModelType, TypeTuple>(
          std::forward<FxnType>(fxn), std::move(new_helper),
          std::forward<HolderTypes>(other_holders)...);
    } else {
        auto v = downcaster<ModelType, TypeTuple>(std::forward<HolderType>(h));
        auto new_helper = helper.add_variant(std::move(v));
        return dispatch_impl_common<ModelType, TypeTuple>(
          std::forward<FxnType>(fxn), std::move(new_helper),
          std::forward<HolderTypes>(other_holders)...);
    }
}

/** @brief Wraps dispatching to a function based on the types contained in a
 *         series of holders.
 *
 *  @tparam ModelType The template template parameter for the Model type to
 *                    downcast to. Must be provided by the user.
 *  @tparam TypeTuple A std::tuple of floating point types that ModelType can
 *                    wrap. Must be provided by the user.
 *  @tparam FxnType The type of the callable to invoke. Will be inferred by the
 *                  compiler.
 *  @tparam HolderTypes The types of the holders to downcast. Will be inferred
 *                      by the compiler.
 *
 *  This is the high-level developer interface for dispatching based on the
 *  types contained in a series of holders. The user of this function provides
 *  the ModelType and TypeTuple template parameters, a callable, and a series of
 *  holders. This function will downcast each holder to a ModelType wrapping one
 *  of the types in TypeTuple. Once all holders have been downcasted the user's
 *  callable will be invoked with the downcasted models and the result will be
 *  returned.
 *
 *  @param[in] fxn The callable to invoke.
 *  @param[in] other_holders The holders to downcast.
 *
 *  @return The return value of the callable.
 *
 *  @throws ??? Any exception thrown by the callable or by dispatch_impl. Same
 *              throw guarantee.
 */
template<template<concepts::FloatingPoint> typename ModelType,
         typename TypeTuple, typename FxnType, typename... HolderTypes>
auto dispatch(FxnType&& fxn, HolderTypes&&... other_holders) {
    if constexpr(sizeof...(other_holders) == 0) {
        // If there are no holders, just call the callable
        return fxn();
    } else {
        using empty_variant_tuple = std::tuple<>;
        DispatchHelper<empty_variant_tuple> helper(empty_variant_tuple{});
        return dispatch_impl<ModelType, TypeTuple>(
          std::forward<FxnType>(fxn), std::move(helper),
          std::forward<HolderTypes>(other_holders)...);
    }
}

} // namespace wtf::detail_