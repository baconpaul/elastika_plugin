#include "ElastikaProcessor.h"
#include "ElastikaEditor.h"
#include "ElastikaBinary.h"

class SapphireLookAndFeel : public juce::LookAndFeel_V4
{
  public:
    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos,
                          float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider &slider) override
    {
        // Setting Bounds and Angles

        auto bounds = juce::Rectangle<int>(x, y, width, height)
                          .toFloat()
                          .reduced(0); // add .reduced for smaller bounds
        rotaryStartAngle =
            -juce::MathConstants<float>::pi * 2.0f * 3.0f / 8.0f; // starting angle in radians
        rotaryEndAngle =
            juce::MathConstants<float>::pi * 2.0f * 3.0f / 8.0f; // finish angle in radians
        auto toAngle =
            rotaryStartAngle +
            sliderPos * (rotaryEndAngle - rotaryStartAngle); // total angle traversed by slider

        // Setting the inner square where the knob will sit

        bounds = bounds.constrainedWithin(
            juce::Rectangle<int>(x, y, juce::jmin(width, height), juce::jmin(width, height))
                .toFloat());
        bounds.setPosition(0, 0);

        // Setting Scaling Values

        auto scaling_val = bounds.getHeight(); // 420 @ 600,600
        auto key_arc = bounds.reduced(scaling_val / 7.0f);
        auto minute_arc = bounds.reduced(scaling_val / 8.4f);
        auto minute_marker_thickness = scaling_val / 120.0f;

        // Creating Gradient

        juce::Point<float> p1(bounds.getCentre());
        juce::Point<float> p2(key_arc.reduced(scaling_val / 19.0f).getTopLeft());
        juce::ColourGradient black_fade(juce::Colours::black, p1, grey, p2, true); // move this

        g.setGradientFill(black_fade);
        g.fillEllipse(bounds);

        // Creating minute markers

        g.setColour(light_grey);

        juce::Point<float> circle_edge(minute_arc.getTopLeft().getX(),
                                       bounds.getCentreY()); // scale around to make full circle
        circle_edge.applyTransform(
            juce::AffineTransform::rotation(-2.0f * juce::MathConstants<float>::pi * 40.0f / 360.0f,
                                            bounds.getCentreX(), bounds.getCentreY()));
        juce::Line<float> horizontal_line(bounds.getCentre(), circle_edge);
        g.drawLine(horizontal_line, minute_marker_thickness);

        for (int degree = 0; degree < 26; degree++)
        {
            circle_edge.applyTransform(juce::AffineTransform::rotation(
                2.0f * juce::MathConstants<float>::pi * 10.0f / 360.0f, bounds.getCentreX(),
                bounds.getCentreY()));
            horizontal_line.setEnd(circle_edge);
            g.drawLine(horizontal_line, minute_marker_thickness);
        }

        // Creating knob face

        g.setColour(grey);
        g.fillEllipse(key_arc);

        // Creating Knob Indicator

        auto arcRadius = key_arc.getWidth() / 2.0f;

        juce::Point<float> LineEndPoint(
            bounds.getCentreX() +
                arcRadius * std::cos(toAngle - juce::MathConstants<float>::halfPi),
            bounds.getCentreY() +
                arcRadius * std::sin(toAngle - juce::MathConstants<float>::halfPi));
        auto LineStartX = (bounds.getCentre().getX() + LineEndPoint.getX()) / 2.0f;
        auto LineStartY = (bounds.getCentre().getY() + LineEndPoint.getY()) / 2.0f;
        juce::Point<float> LinesStartPoint(LineStartX, LineStartY);

        juce::Line<float> marker_line(LinesStartPoint, LineEndPoint);

        g.setColour(light_grey);
        g.drawLine(marker_line, minute_marker_thickness * 2);
    }

    juce::Slider::SliderLayout getSliderLayout(juce::Slider &slider) override
    {
        juce::Slider::SliderLayout layout;

        layout.textBoxBounds = slider.getBounds();
        layout.sliderBounds = slider.getBounds();

        return layout;
    }

  private:
    juce::Colour grey = juce::Colour::fromRGB(34, 34, 34);
    juce::Colour light_grey = juce::Colour::fromRGB(136, 136, 136);
    juce::Colour dark_grey = juce::Colour::fromRGB(25, 25, 25);
};

//==============================================================================
ElastikaEditor::ElastikaEditor(ElastikaAudioProcessor &p)
    : juce::AudioProcessorEditor(&p), processor(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    lnf = std::make_unique<SapphireLookAndFeel>();
    setLookAndFeel(lnf.get());

    setSize(300, 600);
    setResizable(true, true);

    std::unique_ptr<juce::XmlElement> xml = juce::XmlDocument::parse(ElastikaBinary::elastika_svg);
    background = juce::Drawable::createFromSVG(*xml);
    // background->setTransform(juce::RectanglePlacement{128}.getTransformToFit(svgDefaultBounds,
    // getLocalBounds().toFloat())); tosvg =
    // juce::AffineTransform::fromTargetPoints(juce::Point{0.f, 0.f}, ob.getTopLeft().toFloat(),
    // juce::Point{60.96f, 0.f}, ob.getTopRight().toFloat(), juce::Point{60.96f, 128.5f},
    // ob.getBottomRight().toFloat()); addAndMakeVisible(*background);
    //  Find knobs.
    std::cout << "Top tag name: " << xml->getTagName() << std::endl;
    auto controls = xml->getChildByAttribute("id", "ControlLayer");
    if (!controls)
    {
        std::cout << "Big failure!" << std::endl;
        return;
    }
    for (const auto *control : controls->getChildWithTagNameIterator("circle"))
    {
        std::cout << "id: " << control->getStringAttribute("id")
                  << "; x: " << control->getStringAttribute("cx")
                  << "; y: " << control->getStringAttribute("cy") << std::endl;
        const juce::String &id = control->getStringAttribute("id");
        if (id.endsWith("slider"))
        {
            float cx = control->getStringAttribute("cx").getFloatValue();
            float cy = control->getStringAttribute("cy").getFloatValue();
            // knobs.emplace_back(cx, cy);
            auto kn = std::make_unique<juce::Slider>();
            kn->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
            kn->setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
            background->addAndMakeVisible(*kn);
            kn->setSize(8, 8);
            // 1.05 offset determined through trial and error.
            juce::Point<float> real{cx + 1.05f, cy};
            juce::Point<int> rounded = real.toInt();
            // rounded.applyTransform(tosvg);
            // real.applyTransform(tosvg);
            std::cout << "Rounded diff: " << (real - rounded.toFloat()).toString() << std::endl;
            kn->setCentrePosition(rounded);
            kn->setTransform(juce::AffineTransform::translation(real.getX() - rounded.getX(),
                                                                real.getY() - rounded.getY()));
            // kn->setTransform(tosvg);
            // kn->setTransform(tosvg.translated(cx - 4.f, cy - 4.f));
            // addAndMakeVisible(*kn);
            knobs.push_back(std::move(kn));
            knob_positions.emplace_back(cx, cy);
            std::cout << "Emplaced knob " << id << std::endl;
        }
    }

#if 0
      auto addKnob = [this](const std::string label, auto &paramRef) {
        auto kn = std::make_unique<juce::Slider>();
        kn->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        kn->setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
        addAndMakeVisible(*kn);

        auto spa = std::make_unique<juce::SliderParameterAttachment>(paramRef, *kn);
        attachments.push_back(std::move(spa));
        knobs.push_back(std::move(kn));
        auto lb = std::make_unique<juce::Label>(label, label);
        lb->setColour(juce::Label::textColourId, juce::Colours::black);
        lb->setJustificationType(juce::Justification::centred);
        addAndMakeVisible(*lb);
        labels.push_back(std::move(lb));
    };
    addKnob("Friction", *(processor.friction));
    addKnob("Span", *(processor.span));
    addKnob("Curl", *(processor.curl));
    addKnob("Mass", *(processor.mass));
    addKnob("Drive", *(processor.drive));
    addKnob("Gain", *(processor.gain));
    addKnob("InTilt", *(processor.inputTilt));
    addKnob("OutTilt", *(processor.outputTilt));
#endif

    addAndMakeVisible(*background);
    resized();
}

ElastikaEditor::~ElastikaEditor() {}

//==============================================================================
void ElastikaEditor::paintOverChildren(juce::Graphics &g)
{
    // juce::ColourGradient cg(juce::Colour(240, 220, 220), 0, 0, juce::Colour(220, 220, 220), 0,
    // getHeight(),
    //                         false);
    // g.setGradientFill(cg);
    // g.fillRect(getLocalBounds());
    // g.setColour(juce::Colours::black);
    // g.setFont(30);
    // g.drawText("Elastika", getLocalBounds(), juce::Justification::centredTop);
    // background->draw(g, 0.0);
#if 0
  g.setColour(juce::Colour::fromRGB(0, 0, 0));
  for (const auto& knob : knobs) {
    juce::Rectangle<float> ul{knob.first, knob.second, 2.f, 2.f};
    ul = getLocalArea(background.get(), ul);
    g.fillRect(ul);
    std::cout << "Filling region from ("
              << knob.first - 1.f << "," << knob.second - 1.f << ") to "
              << knob.first + 1.f << "," << knob.second + 1.f << ")" << std::endl;
    //g.fillEllipse(knob.first - 2.f, knob.second - 2.f, 2.f, 2.f);
  }
#endif
}

void ElastikaEditor::resized()
{
    if (background)
    {
        // background->setTransform(juce::RectanglePlacement{128}.getTransformToFit(svgDefaultBounds,
        // getLocalBounds().toFloat()));
        background->setTransformToFit(getLocalBounds().toFloat(), juce::RectanglePlacement());
#if 0
    for (int i = 0; i < knobs.size(); i++) {
      auto pos = knob_positions[i];
      //juce::Rectangle<int> ul{int(pos.first - 3.5f), int(pos.second - 3.5f), 8, 8};
      //juce::Rectangle<float> real{pos.first - 3.5f, pos.second - 3.5f, 8, 8};
      //ul = getLocalArea(background.get(), ul);
      //real = getLocalArea(background.get(), real);
      //std::cout << "Real location bounds: " << real.toString() << std::endl;
      //knobs[i]->setBounds(ul);
      knobs[i]->setSize(8, 8);
      auto ob = background->getDrawableBounds();
      juce::AffineTransform t1 = juce::AffineTransform::fromTargetPoints(juce::Point{0.f, 0.f}, ob.getTopLeft(), juce::Point{60.96f, 0.f}, ob.getTopRight(), juce::Point{60.96f, 128.5f}, ob.getBottomRight());
      juce::Point<int> c1{int(pos.first), int(pos.second)};
      juce::Point<float> c2{pos.first, pos.second};
      c1.applyTransform(t1);
      //std::cout << "Original position: " << c2.toString() << std::endl;
      c2.applyTransform(tosvg);
      //std::cout << "Transformed position: " << c2.toString() << std::endl;
      //c1 = getLocalPoint(background.get(), c1);
      //c2 = getLocalPoint(background.get(), c2);
      knobs[i]->setTransform(juce::AffineTransform::translation(c2.getX() - 4.f, c2.getY() - 4.f));
      //knobs[i]->setTransform(juce::AffineTransform::translation(c2.getX(), c2.getY()).followedBy(background->getTransform()));
      //knobs[i]->setTransform(juce::AffineTransform::translation(c2.getX() - 4.f, c2.getY() - 4.f).followedBy(background->getTransform()));
      //knobs[i]->setCentrePosition(c1);
      //knobs[i]->setTransform(juce::AffineTransform{}.translated(c2.getX() - c1.getX(), c2.getY() - c1.getY()));
      //knobs[i]->setTransform(juce::AffineTransform{}.translated(real.getX() - ul.getX(), real.getY() - ul.getY()));
    }
#endif
    }
#if 0
    auto slsz = getWidth() / 4 * 0.8;
    auto slmg = getWidth() / 4 * 0.1;
    auto p = getLocalBounds().withTrimmedTop(80).withWidth(slsz).withHeight(slsz).translated(slmg, 0);

    for (int i=0; i<knobs.size(); ++i)
    {
        knobs[i]->setBounds(p);
        auto l = p.translated(0, slsz + 2).withHeight(20);
        labels[i]->setBounds(l);;

        p = p.translated(slsz + 2 * slmg, 0);


        if (i == 3)
            p = getLocalBounds().withTrimmedTop(80 + slsz + 30).withWidth(slsz).withHeight(slsz).translated(slmg, 0);
    }
#endif
}
