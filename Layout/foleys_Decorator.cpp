/*
 ==============================================================================
    Copyright (c) 2019-2020 Foleys Finest Audio Ltd. - Daniel Walz
    All rights reserved.

    License for non-commercial projects:

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

    License for commercial products:

    To sell commercial products containing this module, you are required to buy a
    License from https://foleysfinest.com/developer/pluginguimagic/

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


namespace foleys
{

void Decorator::drawDecorator (juce::Graphics& g, juce::Rectangle<int> bounds)
{
    juce::Graphics::ScopedSaveState stateSave (g);

    auto boundsf = bounds.reduced (margin).toFloat();

    {
        juce::Graphics::ScopedSaveState save (g);
        g.setColour (backgroundColour);

        if (backgroundFill.size() > 1)
            g.setGradientFill (juce::ColourGradient::vertical (backgroundFill.getFirst(), backgroundFill.getLast(), bounds));

        if (radius > 0.0f)
            g.fillRoundedRectangle (boundsf, radius);
        else
            g.fillRect (bounds);
    }

    if (! backgroundImage.isNull())
    {
        juce::Graphics::ScopedSaveState save (g);
        g.setOpacity (backgroundAlpha);
        g.drawImage (backgroundImage, boundsf, backgroundPlacement);
    }

    if (border > 0.0f)
    {
        g.setColour (borderColour);

        if (radius > 0.0f)
            g.drawRoundedRectangle (boundsf, radius, border);
        else
            g.drawRect (bounds, border);
    }

    if (caption.isNotEmpty())
    {
        auto clientBounds = getClientBounds (bounds);

        g.setColour (captionColour);

        g.setFont (juce::Font (captionSize * 0.8f).boldened());
        g.drawFittedText (caption, clientBounds.caption, justification.getOnlyHorizontalFlags(), 1);
    }
}

juce::String Decorator::getTabCaption (const juce::String& defaultName) const
{
    return tabCaption.isNotEmpty() ? tabCaption
                                   : caption.isNotEmpty() ? caption
                                                          : defaultName;
}

juce::Colour Decorator::getTabColour() const
{
    return tabColour;
}

Decorator::ClientBounds Decorator::getClientBounds (juce::Rectangle<int> overallBounds) const
{
    auto box = overallBounds.reduced (juce::roundToInt (margin + padding));
    juce::Rectangle<int> captionBox;

    if (caption.isNotEmpty())
    {
        if (justification.getOnlyVerticalFlags() & juce::Justification::top)
            captionBox = box.removeFromTop (int (captionSize));
        else if (justification.getOnlyVerticalFlags() & juce::Justification::bottom)
            captionBox = box.removeFromBottom (int (captionSize));
        else
        {
            juce::Font f (captionSize * 0.8f, juce::Font::bold);
            auto w = f.getStringWidth (caption);

            if (justification.getOnlyHorizontalFlags() & juce::Justification::left)
                captionBox = box.removeFromLeft (w);
            else if (justification.getOnlyHorizontalFlags() & juce::Justification::right)
                captionBox = box.removeFromRight (w);
        }
    }

    return { box, captionBox };
}

void Decorator::configure (MagicGUIBuilder& builder, const juce::ValueTree& node)
{
    auto& stylesheet     = builder.getStylesheet();

    auto bg = builder.getStyleProperty (IDs::backgroundColour, node);
    if (! bg.isVoid())
        backgroundColour = Stylesheet::parseColour (bg.toString());

    auto bcVar = builder.getStyleProperty (IDs::borderColour, node);
    if (! bcVar.isVoid())
        borderColour = Stylesheet::parseColour (bcVar.toString());

    auto borderVar = builder.getStyleProperty (IDs::border, node);
    if (! borderVar.isVoid())
        border = static_cast<float> (borderVar);

    auto marginVar = builder.getStyleProperty (IDs::margin, node);
    if (! marginVar.isVoid())
        margin = static_cast<float> (marginVar);

    auto paddingVar = builder.getStyleProperty (IDs::padding, node);
    if (! paddingVar.isVoid())
        padding = static_cast<float> (paddingVar);

    auto radiusVar = builder.getStyleProperty (IDs::radius, node);
    if (! radiusVar.isVoid())
        radius = static_cast<float> (radiusVar);

    caption    = node.getProperty (IDs::caption, juce::String());
    tabCaption = node.getProperty (IDs::tabCaption, juce::String());
    auto tc    = builder.getStyleProperty (IDs::tabColour, node);
    if (! tc.isVoid())
        tabColour = Stylesheet::parseColour (tc.toString());

    auto sizeVar = builder.getStyleProperty (IDs::captionSize, node);
    if (! sizeVar.isVoid())
        captionSize = static_cast<float> (sizeVar);

    auto ccVar = builder.getStyleProperty (IDs::captionColour, node);
    if (! ccVar.isVoid())
        captionColour = Stylesheet::parseColour (ccVar.toString());

    auto placementVar = builder.getStyleProperty (IDs::captionPlacement, node);
    if (! placementVar.isVoid())
        justification = juce::Justification (makeJustificationsChoices()[placementVar.toString()]);
    else
        justification = juce::Justification::centredTop;

    backgroundImage = stylesheet.getBackgroundImage (node);
    backgroundFill  = stylesheet.getBackgroundGradient (node);

    auto alphaVar = builder.getStyleProperty (IDs::backgroundAlpha, node);
    if (! alphaVar.isVoid())
        backgroundAlpha = static_cast<float> (alphaVar);

    auto backPlacement = builder.getStyleProperty (IDs::imagePlacement, node);
    if (! backPlacement.isVoid())
    {
        if (backPlacement.toString() == IDs::imageStretch)
            backgroundPlacement = juce::RectanglePlacement::stretchToFit;
        else if (backPlacement.toString() == IDs::imageFill)
            backgroundPlacement = juce::RectanglePlacement::fillDestination;
        else if (backPlacement.toString() == IDs::imageCentred)
            backgroundPlacement = juce::RectanglePlacement::centred;
    }
}

void Decorator::reset()
{
    backgroundColour = juce::Colours::darkgrey;
    borderColour     = juce::Colours::silver;

    margin  = 5.0f;
    padding = 5.0f;
    border  = 0.0f;
    radius  = 5.0f;

    caption.clear();
    justification = juce::Justification::centredTop;
    captionSize   = 20.0f;
    captionColour = juce::Colours::silver;

    tabCaption.clear();
    tabColour = juce::Colours::darkgrey;

    backgroundImage = juce::Image();
    backgroundAlpha = 1.0f;
    backgroundPlacement = juce::RectanglePlacement::centred;
    backgroundFill.clear();
}

}
