<?php

namespace App\Http\Requests\Api;

use App\Models\Device;
use Illuminate\Contracts\Validation\ValidationRule;
use Illuminate\Foundation\Http\FormRequest;
use Illuminate\Validation\Rule;

class StoreFingerprintRequest extends FormRequest
{
    /**
     * Determine if the user is authorized to make this request.
     */
    public function authorize(): bool
    {
        return true;
    }

    /**
     * Get the validation rules that apply to the request.
     *
     * @return array<string, ValidationRule|array|string>
     */
    public function rules(): array
    {
        $device = Device::firstOrCreate();
        $kapasitas = $device?->kapasitas ?? 1000; // Default to 1000 if not found

        return [
            'fingerprint_ids' => ['required', 'array'],
            'fingerprint_ids.*' => [
                'integer',
                'min:1',
                "max:$kapasitas", // Ensure IDs are within kapasitas range
                Rule::exists('fingerprints', 'id')->where(fn ($query) =>
                    $query->where('id_guru', $this->route('guru')->id)
                ),
            ],
        ];
    }
}
