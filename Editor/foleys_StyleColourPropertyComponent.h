/*
 ==============================================================================
    Copyright (c) 2019 Foleys Finest Audio Ltd. - Daniel Walz
    All rights reserved.

    Redistribution and use in source and binary forms, with or without modification,
    are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
    IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
    INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
    OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
    OF THE POSSIBILITY OF SUCH DAMAGE.
 ==============================================================================
 */

#pragma once

namespace foleys
{

class StyleColourPropertyComponent  : public StylePropertyComponent,
                                      private juce::Value::Listener
{
public:
    StyleColourPropertyComponent (MagicBuilder& builderToUse, juce::Identifier propertyToUse, juce::ValueTree& nodeToUse);
    ~StyleColourPropertyComponent() = default;

    void refresh() override;

private:

    class ColourPanel : public juce::Component, private juce::ChangeListener
    {
    public:
        ColourPanel (juce::Value value, juce::Colour colour);
        void resized() override;
    private:
        void changeListenerCallback (juce::ChangeBroadcaster*) override;

        class ColourSelectorWithSwatches  : public juce::ColourSelector
        {
        public:
            ColourSelectorWithSwatches();

            int getNumSwatches() const override                                   { return numSwatches; }
            juce::Colour getSwatchColour (int index) const override               { return swatchColours [index]; }
            void setSwatchColour (int index, const juce::Colour& colour) override { swatchColours [index] = colour; }

        private:
            const int numSwatches = 20;
            static std::vector<juce::Colour> swatchColours;

            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ColourSelectorWithSwatches)
        };

        juce::Value                 value;
        juce::TextButton            close {"X"};
        ColourSelectorWithSwatches  selector;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ColourPanel)
    };

    void valueChanged (juce::Value& value) override;

    void setColourDisplay (juce::Colour colour);

    void getLookAndFeelColourFallback();

    MouseLambdas mouseEvents;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StyleColourPropertyComponent)
};


} // namespace foleys
