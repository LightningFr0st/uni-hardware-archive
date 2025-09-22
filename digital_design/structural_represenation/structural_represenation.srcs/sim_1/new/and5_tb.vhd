----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 09/22/2025 09:58:30 PM
-- Design Name: 
-- Module Name: and5_tb - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity and5_tb is
--  Port ( );
end and5_tb;

architecture Behavioral of and5_tb is

component and5_struct  
    Port ( A : in STD_LOGIC_VECTOR(4 downto 0);
       Q : out STD_LOGIC);
end component;

component and5_beh  
    Port ( A : in STD_LOGIC_VECTOR(4 downto 0);
       Q : out STD_LOGIC);
end component;

signal test_vector : std_logic_vector (4 downto 0);

signal q_beh : std_logic;
signal q_struct : std_logic;

signal error : std_logic;

constant period : time := 25 ns;

begin

    and_beh: and5_beh port map(
        a => test_vector,
        q => q_beh
     );
     
     and_struct: and5_struct port map(
        a => test_vector,
        q => q_struct
     );
     
     error <= q_struct xor q_beh;

    tb_proc : process
        variable error_count : integer := 0;
     begin
        
        report "Starting testbench...";
        
        for i in 0 to 31 loop
            test_vector <= std_logic_vector(to_unsigned(i, test_vector'length));
            wait for period;
            
            if error = '1' then
                error_count := error_count + 1;
                report "ERROR at test vector: " & integer'image(i);
            end if;
        end loop;
        
        if error_count = 0 then
            report "ALL TESTS PASSED! Both implementations are equivalent." severity note;
        else
            report "TEST FAILED: "& integer'image(error_count) severity note;
        end if;
        
        report "End of simulation" severity failure;
     
     end process;

end Behavioral;
