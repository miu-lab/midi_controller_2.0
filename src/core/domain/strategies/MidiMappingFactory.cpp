#include "core/domain/strategies/MidiMappingFactory.hpp"

std::unique_ptr<IMidiMappingStrategy> MidiMappingFactory::create(StrategyType type) {
    switch (type) {
    case StrategyType::ABSOLUTE:
        return createAbsolute(0, 127);

    case StrategyType::RELATIVE_BINARY:
        return createRelative(5.0f, RelativeMappingStrategy::EncodingType::BINARY_OFFSET);

    case StrategyType::RELATIVE_SIGNED:
        return createRelative(5.0f, RelativeMappingStrategy::EncodingType::SIGNED_BIT);

    case StrategyType::RELATIVE_SIGNED2:
        return createRelative(5.0f, RelativeMappingStrategy::EncodingType::SIGNED_2);

    case StrategyType::RELATIVE_INCREMENT:
        return createRelative(5.0f, RelativeMappingStrategy::EncodingType::INCREMENT_TYPE);

    case StrategyType::DYNAMIC_RANGE:
        return createDynamicRange();

    default:
        return createAbsolute(0, 127);
    }
}

std::unique_ptr<AbsoluteMappingStrategy> MidiMappingFactory::createAbsolute(int32_t minPhysical,
                                                                            int32_t maxPhysical,
                                                                            bool clamp) {
    return std::make_unique<AbsoluteMappingStrategy>(minPhysical, maxPhysical, clamp);
}

std::unique_ptr<RelativeMappingStrategy> MidiMappingFactory::createRelative(
    float sensitivity, RelativeMappingStrategy::EncodingType encoding, bool acceleration) {
    return std::make_unique<RelativeMappingStrategy>(sensitivity, encoding, acceleration);
}

std::unique_ptr<DynamicRangeMappingStrategy> MidiMappingFactory::createDynamicRange(
    int32_t initialMin, int32_t initialMax, uint32_t resetThreshold) {
    return std::make_unique<DynamicRangeMappingStrategy>(initialMin, initialMax, resetThreshold);
}
