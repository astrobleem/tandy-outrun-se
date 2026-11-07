// Copyright Chris White; see retained Cannonball license.
// Exact pinned bodies with DOS16 widen-before-multiply adaptations.
void OInputs::digital_steering()
{
    // ------------------------------------------------------------------------
    // STEERING
    // ------------------------------------------------------------------------
    if (input.is_pressed(Input::LEFT))
    {
        // Recentre wheel immediately if facing other way
        if (input_steering > STEERING_CENTRE) input_steering = STEERING_CENTRE;

        input_steering -= steering_inc;
        if (input_steering < STEERING_MIN) input_steering = STEERING_MIN;
    }
    else if (input.is_pressed(Input::RIGHT))
    {
        // Recentre wheel immediately if facing other way
        if (input_steering < STEERING_CENTRE) input_steering = STEERING_CENTRE;

        input_steering += steering_inc;
        if (input_steering > STEERING_MAX) input_steering = STEERING_MAX;
    }
    // Return steering to centre if nothing pressed
    else
    {
        if (input_steering < STEERING_CENTRE)
        {
            input_steering += steering_inc;
            if (input_steering > STEERING_CENTRE)
                input_steering = STEERING_CENTRE;
        }
        else if (input_steering > STEERING_CENTRE)
        {
            input_steering -= steering_inc;
            if (input_steering < STEERING_CENTRE)
                input_steering = STEERING_CENTRE;
        }
    }
}

void OInputs::digital_pedals()
{
    // ------------------------------------------------------------------------
    // ACCELERATION
    // ------------------------------------------------------------------------

    if (input.is_pressed(Input::ACCEL))
    {
        input_acc += acc_inc;
        if (input_acc > 0xFF) input_acc = 0xFF;
    }
    else
    {
        input_acc -= acc_inc;
        if (input_acc < 0) input_acc = 0;
    }

    // ------------------------------------------------------------------------
    // BRAKE
    // ------------------------------------------------------------------------

    if (input.is_pressed(Input::BRAKE))
    {
        input_brake += brake_inc;
        if (input_brake > 0xFF) input_brake = 0xFF;
    }
    else
    {
        input_brake -= brake_inc;
        if (input_brake < 0) input_brake = 0;
    }
}

void OInputs::adjust_inputs()
{
    // Cap Steering Value
    if (input_steering < STEERING_MIN) input_steering = STEERING_MIN;
    else if (input_steering > STEERING_MAX) input_steering = STEERING_MAX;

    if (crash_input)
    {
        crash_input--;
        int16_t d0 = (((int32_t)input_steering - 0x80) * 0x100L) / 0x70;
        if (d0 > 0x7F) d0 = 0x7F;
        else if (d0 < -0x7F) d0 = -0x7F;
        steering_adjust = ocrash.crash_counter ? 0 : d0;
    }
    else
    {
        // no_crash1:
        int16_t d0 = input_steering - steering_old;
        steering_old = input_steering;
        steering_change += d0;
        d0 = steering_change < 0 ? -steering_change : steering_change;

        // Note the below line "if (d0 > 2)" causes a bug in the original game
        // whereby if you hold the wheel to the left whilst stationary, then accelerate the car will veer left even
        // when the wheel has been centered
        if (config.engine.fix_bugs || d0 > 2)
        {
            steering_change = 0;
            // Convert input steering value to internal value
            d0 = (((int32_t)input_steering - 0x80) * 0x100L) / 0x70;
            if (d0 > 0x7F) d0 = 0x7F;
            else if (d0 < -0x7F) d0 = -0x7F;
            steering_adjust = ocrash.crash_counter ? 0 : d0;
        }
    }

    // Cap & Adjust Acceleration Value
    int16_t acc = input_acc;
    if (acc > PEDAL_MAX) acc = PEDAL_MAX;
    else if (acc < PEDAL_MIN) acc = PEDAL_MIN;
    acc_adjust = (((int32_t)acc - 0x30) * 0x100L) / 0x61;

    // Cap & Adjust Brake Value
    int16_t brake = input_brake;
    if (brake > PEDAL_MAX) brake = PEDAL_MAX;
    else if (brake < PEDAL_MIN) brake = PEDAL_MIN;
    brake_adjust = (((int32_t)brake - 0x30) * 0x100L) / 0x61;
}
