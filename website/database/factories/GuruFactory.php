<?php

namespace Database\Factories;

use App\Enums\GenderEnum;
use App\Models\Guru;
use Illuminate\Database\Eloquent\Factories\Factory;
use Illuminate\Support\Str;

/**
 * @extends Factory<Guru>
 */
class GuruFactory extends Factory
{
    /**
     * Define the model's default state.
     *
     * @return array<string, mixed>
     */
    public function definition(): array
    {
        return [
            'nip' => fake()->numerify('##################'),
            'nama' => fake()->name(),
            'jenis_kelamin' => fake()->randomElement(GenderEnum::values()),
            'no_hp' => fake()->phoneNumber(),
            'alamat' => fake()->address(),
        ];
    }
}
