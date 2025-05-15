<?php

namespace Database\Factories;

use App\Enums\HariEnum;
use Illuminate\Database\Eloquent\Factories\Factory;

/**
 * @extends \Illuminate\Database\Eloquent\Factories\Factory<\App\Models\Jadwal>
 */
class JadwalFactory extends Factory
{
    /**
     * Define the model's default state.
     *
     * @return array<string, mixed>
     */
    public function definition(): array
    {
        return [
            'hari' => fake()->randomElement(HariEnum::values()),
            'jam_masuk' => '07:00:00',
            'jam_keluar' => '16:00:00',
            'toleransi_keterlambatan' => 30,
        ];
    }
}
